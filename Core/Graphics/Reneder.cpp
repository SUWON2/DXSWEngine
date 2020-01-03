#include <d3dcompiler.h>

#include "Reneder.h"
#include "../../Common/Define.h"
#include "../../Common/Setting.h"
#include "../../common/DirectXMath.h"

using namespace DirectX;

static void CompileShader(const wchar_t* fileName, const char* entryPoint, const char* shaderModel, ID3DBlob** outShaderByteCode);

Reneder::Reneder()
{
}

Reneder::~Reneder()
{
	ReleaseCOM(mConstantBuffer);
	ReleaseCOM(mPixelShader);
	ReleaseCOM(mInputLayout);
	ReleaseCOM(mVertexShader);
}

void Reneder::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	ASSERT(device != nullptr, "The device must not be null");
	ASSERT(deviceContext != nullptr, "The deviceContext must not be null");

	mDevice = device;
	mDeviceContext = deviceContext;
	mCamera = std::make_unique<Camera>();
	mMeshManager = std::make_unique<MeshManager>(device, deviceContext);

	// HACK: Initialize vertex shader and pixel shader
	{
		ID3DBlob* vsByteCode = nullptr;
		CompileShader(L"./Shaders/BasicShaderVS.hlsl", "VS", "vs_4_0", &vsByteCode);
		HR(device->CreateVertexShader(vsByteCode->GetBufferPointer(), vsByteCode->GetBufferSize(), nullptr, &mVertexShader));

		const D3D11_INPUT_ELEMENT_DESC inputLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		constexpr UINT inputLayoutCount = sizeof(inputLayout) / sizeof(D3D11_INPUT_ELEMENT_DESC);

		HR(device->CreateInputLayout(inputLayout, inputLayoutCount
			, vsByteCode->GetBufferPointer(), vsByteCode->GetBufferSize(), &mInputLayout));
		ReleaseCOM(vsByteCode);

		deviceContext->IASetInputLayout(mInputLayout);

		ID3DBlob* psByteCode = nullptr;
		CompileShader(L"./Shaders/BasicShaderPS.hlsl", "PS", "ps_4_0", &psByteCode);

		HR(device->CreatePixelShader(psByteCode->GetBufferPointer(), psByteCode->GetBufferSize(), nullptr, &mPixelShader));
		ReleaseCOM(psByteCode);

		// HACK: ShaderManager를 만들 때 동안만 임시로 처리한다.
		deviceContext->VSSetShader(mVertexShader, nullptr, 0);
		deviceContext->PSSetShader(mPixelShader, nullptr, 0);
	}

	// HACK: Initialize matrixs
	{
		struct WVP
		{
			XMMATRIX mWorld = {};
			XMMATRIX mView = {};
			XMMATRIX mProjection = {};
		};

		D3D11_BUFFER_DESC constantBufferDesc = {};
		constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		constantBufferDesc.ByteWidth = sizeof(WVP);
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.CPUAccessFlags = 0;
		HR(device->CreateBuffer(&constantBufferDesc, nullptr, &mConstantBuffer));

		deviceContext->VSSetConstantBuffers(0, 1, &mConstantBuffer);
	}
}

void Reneder::Draw()
{
	// HACK: Update constant buffer
	{
		struct WVP
		{
			XMMATRIX mWorld = {};
			XMMATRIX mView = {};
			XMMATRIX mProjection = {};
		};

		const XMMATRIX matWorld = XMMatrixTranslation(0.0f, 0.0f, 0.0f);

		// TODO: 책에 나온 방법으로 내가 생각한 것과 달리 회전 결과가 다르고 처리하는 연산도 많음
		// , 따라서 이 방식의 결과를 이해한 뒤 코드를 지우면 될듯
		/*{
			const XMFLOAT3& cameraPos = mCamera->GetPosition();
			const XMFLOAT3& cameraRot = mCamera->GetRotation();

			const XMVECTOR vecEye = XMVectorSet(cameraPos.x, cameraPos.y, cameraPos.z, 0.0f);
			const XMVECTOR vecLookAt = XMVectorSet(cameraPos.x, cameraPos.y, 1.0f + cameraPos.z, 0.0f);
			const XMVECTOR vecUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

			XMVECTOR vecZAxis = XMVector3Normalize(vecLookAt - vecEye);
			XMVECTOR vecXAxis = XMVector3Normalize(XMVector3Cross(vecUp, vecZAxis));
			XMVECTOR vecYAxis = XMVector3Cross(vecZAxis, vecXAxis);

			const XMMATRIX matRotY = XMMatrixRotationAxis(vecXAxis, -(cameraRot.x));
			vecXAxis = XMVector3Normalize(XMVector3TransformNormal(vecXAxis, matRotY));
			vecYAxis = XMVector3Normalize(XMVector3TransformNormal(vecYAxis, matRotY));
			vecZAxis = XMVector3Normalize(XMVector3TransformNormal(vecZAxis, matRotY));

			const XMMATRIX matRotX = XMMatrixRotationY(-(cameraRot.y));
			vecYAxis = XMVector3Normalize(XMVector3TransformNormal(vecYAxis, matRotX));
			vecZAxis = XMVector3Normalize(XMVector3TransformNormal(vecZAxis, matRotX));

			vecZAxis = XMVector3Normalize(vecZAxis);
			vecYAxis = XMVector3Normalize(XMVector3Cross(vecZAxis, vecXAxis));
			vecXAxis = XMVector3Cross(vecYAxis, vecZAxis);
		}*/

		XMMATRIX matView;
		mCamera->LoadViewMatrix(&matView);

		const XMMATRIX matProjection = XMMatrixPerspectiveFovLH(XM_PIDIV4
			, Setting::Get().GetWidth() / static_cast<float>(Setting::Get().GetHeight()), 1.0f, 100.0f);

		WVP wvp;
		wvp.mWorld = XMMatrixTranspose(matWorld);
		wvp.mView = XMMatrixTranspose(matView);
		wvp.mProjection = XMMatrixTranspose(matProjection);

		mDeviceContext->UpdateSubresource(mConstantBuffer, 0, nullptr, &wvp, 0, 0);
	}

	mMeshManager->Draw();
}

void CompileShader(const wchar_t* fileName, const char* entryPoint, const char* shaderModel, ID3DBlob** outShaderByteCode)
{
	ASSERT(fileName != nullptr, "The fileName must not be null");
	ASSERT(entryPoint != nullptr, "The entryPoint must not be null");
	ASSERT(shaderModel != nullptr, "The shaderModel must not be null");
	ASSERT(outShaderByteCode != nullptr, "The outShaderByteCode must not be null");

	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(DEBUG) | defined(_DEBUG)
	shaderFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* errorBlob = nullptr;
	HR(D3DCompileFromFile(fileName, nullptr, nullptr, entryPoint, shaderModel,
		shaderFlags, 0, outShaderByteCode, &errorBlob));

	ASSERT(errorBlob == nullptr, static_cast<const char*>(errorBlob->GetBufferPointer()));
	ReleaseCOM(errorBlob);
}
