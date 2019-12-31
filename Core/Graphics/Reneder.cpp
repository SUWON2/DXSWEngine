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
	ReleaseCOM(mPixelShader);
	ReleaseCOM(mInputLayout);
	ReleaseCOM(mVertexShader);
}

void Reneder::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	ASSERT(device != nullptr, "The device must not be null");
	ASSERT(deviceContext != nullptr, "The deviceContext must not be null");

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

		// HACK: ShaderManager를 만들 때 동안 임시로 처리한다.
		deviceContext->VSSetShader(mVertexShader, nullptr, 0);
		deviceContext->PSSetShader(mPixelShader, nullptr, 0);
	}

	// HACK: Initialize matrixs
	{
		const XMMATRIX world = XMMatrixIdentity();

		const XMVECTOR eye = XMVectorSet(0.0f, 0.5f, -1.0f, 0.0f);
		const XMVECTOR lookAt = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		const XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		const XMMATRIX view = XMMatrixLookAtLH(eye, lookAt, up);

		const XMMATRIX projection = XMMatrixPerspectiveFovLH(XM_PIDIV2
			, Setting::Get().GetWidth() / static_cast<float>(Setting::Get().GetHeight()), 0.01f, 100.0f);

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

		ID3D11Buffer* constantBuffer = nullptr;
		HR(device->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffer));

		WVP wvp;
		wvp.mWorld = XMMatrixTranspose(world);
		wvp.mView = XMMatrixTranspose(view);
		wvp.mProjection = XMMatrixTranspose(projection);
		deviceContext->UpdateSubresource(constantBuffer, 0, nullptr, &wvp, 0, 0);
	
		deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);
		ReleaseCOM(constantBuffer);
	}
}

void Reneder::Draw()
{
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
