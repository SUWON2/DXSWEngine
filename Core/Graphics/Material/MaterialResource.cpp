#define INITGUID // D3DReflect()를 사용하기 위해 정의한다.

#include <d3dcompiler.h>
#include <memory>

#include "MaterialResource.h"
#include "../../../Common/Define.h"
#include "../../../Common/DirectXTex.h"

using namespace DirectX;

static void CompileShader(const char* fileName, const char* entryPoint, const char* shaderModel, ID3DBlob** outShaderByteCode);

MaterialResource::MaterialResource(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: mDevice(device)
	, mDeviceContext(deviceContext)
{
	ASSERT(mDevice != nullptr, "The device must not be null");
	ASSERT(mDeviceContext != nullptr, "The deviceContext must not be null");
}

MaterialResource::~MaterialResource()
{
	for (auto& i : mTextures)
	{
		RELEASE_COM(i.second);
	}

	for (auto& i : mPixelShaders)
	{
		RELEASE_COM(i.second)
	}

	for (auto& i : mVertexShaderBuffers)
	{
		RELEASE_COM(i.second.InputLayout)
		RELEASE_COM(i.second.Interface)
	}
}

size_t MaterialResource::RegisterVertexShader(const char* fileName)
{
	ASSERT(fileName != nullptr, "The fileName msut not be null");

	const auto& foundVertexShader = mVertexShaderBuffers.find(fileName);
	if (foundVertexShader != mVertexShaderBuffers.end())
	{
		return reinterpret_cast<size_t>(&foundVertexShader->first);
	}

	ID3DBlob* vertexShaderByteCode = nullptr;
	CompileShader(fileName, "VS", "vs_4_0", &vertexShaderByteCode);

	ID3D11VertexShader* vertexShader = nullptr;
	HR(mDevice->CreateVertexShader(vertexShaderByteCode->GetBufferPointer(), vertexShaderByteCode->GetBufferSize()
		, nullptr, &vertexShader));

	// ID3D11ShaderReflection을 통해 컴파일된 셰이더의 정보를 가져올 수 있습니다. 더 자세한 내용은 아래 링크를 통해 확인할 수 있습니다.
	// link: https://takinginitiative.wordpress.com/2011/12/11/directx-1011-basic-shader-reflection-automatic-input-layout-creation/
	ID3D11ShaderReflection* vertexShaderReflection = nullptr;
	HR(D3DReflect(vertexShaderByteCode->GetBufferPointer(), vertexShaderByteCode->GetBufferSize()
		, IID_ID3D11ShaderReflection, reinterpret_cast<void**>(&vertexShaderReflection)));

	D3D11_SHADER_DESC shaderDesc;
	vertexShaderReflection->GetDesc(&shaderDesc);

	const int inputLayoutDescCount = shaderDesc.InputParameters;
	auto inputLayoutDescList = std::make_unique<D3D11_INPUT_ELEMENT_DESC[]>(inputLayoutDescCount);

	for (int i = 0; i < inputLayoutDescCount; ++i)
	{
		// 버텍스 셰이더의 입력 파라미터를 가져온다.
		D3D11_SIGNATURE_PARAMETER_DESC parameterDesc;
		vertexShaderReflection->GetInputParameterDesc(i, &parameterDesc);

		D3D11_INPUT_ELEMENT_DESC& inputLayout = inputLayoutDescList[i];
		inputLayout.SemanticName = parameterDesc.SemanticName;
		inputLayout.SemanticIndex = parameterDesc.SemanticIndex;
		inputLayout.InputSlot = 0;
		inputLayout.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		inputLayout.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputLayout.InstanceDataStepRate = 0;

		// D3D 포맷을 DXGI 포맷으로 대응시킨다.
		if (parameterDesc.Mask == 1)
		{
			if (parameterDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
			{
				inputLayout.Format = DXGI_FORMAT_R32_UINT;
			}
			else if (parameterDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
			{
				inputLayout.Format = DXGI_FORMAT_R32_SINT;
			}
			else if (parameterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
			{
				inputLayout.Format = DXGI_FORMAT_R32_FLOAT;
			}
		}
		else if (parameterDesc.Mask <= 3)
		{
			if (parameterDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
			{
				inputLayout.Format = DXGI_FORMAT_R32G32_UINT;
			}
			else if (parameterDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
			{
				inputLayout.Format = DXGI_FORMAT_R32G32_SINT;
			}
			else if (parameterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
			{
				inputLayout.Format = DXGI_FORMAT_R32G32_FLOAT;
			}
		}
		else if (parameterDesc.Mask <= 7)
		{
			if (parameterDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
			{
				inputLayout.Format = DXGI_FORMAT_R32G32B32_UINT;
			}
			else if (parameterDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
			{
				inputLayout.Format = DXGI_FORMAT_R32G32B32_SINT;
			}
			else if (parameterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
			{
				inputLayout.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
		}
		else if (parameterDesc.Mask <= 15)
		{
			if (parameterDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
			{
				inputLayout.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			}
			else if (parameterDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
			{
				inputLayout.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			}
			else if (parameterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
			{
				inputLayout.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}
		}
	}

	ID3D11InputLayout* inputLayout = nullptr;
	HR(mDevice->CreateInputLayout(inputLayoutDescList.get(), inputLayoutDescCount
		, vertexShaderByteCode->GetBufferPointer(), vertexShaderByteCode->GetBufferSize(), &inputLayout));

	RELEASE_COM(vertexShaderReflection);
	RELEASE_COM(vertexShaderByteCode);

	mVertexShaderBuffers.insert(std::make_pair(fileName, VertexShaderBuffer{ vertexShader, inputLayout }));

	return reinterpret_cast<size_t>(&mVertexShaderBuffers.find(fileName)->first);
}

size_t MaterialResource::RegisterPixelShader(const char* fileName)
{
	ASSERT(fileName != nullptr, "The fileName msut not be null");

	const auto& foundPixelShader = mPixelShaders.find(fileName);
	if (foundPixelShader != mPixelShaders.end())
	{
		return reinterpret_cast<size_t>(&foundPixelShader->first);
	}

	ID3DBlob* pixelShaderByteCode = nullptr;
	CompileShader(fileName, "PS", "ps_4_0", &pixelShaderByteCode);

	ID3D11PixelShader* pixelShader = nullptr;
	HR(mDevice->CreatePixelShader(pixelShaderByteCode->GetBufferPointer()
		, pixelShaderByteCode->GetBufferSize(), nullptr, &pixelShader));

	RELEASE_COM(pixelShaderByteCode);

	mPixelShaders.insert(std::make_pair(fileName, pixelShader));

	return reinterpret_cast<size_t>(&mPixelShaders.find(fileName)->first);
}

size_t MaterialResource::RegisterTexture(const char* fileName)
{
	ASSERT(fileName != nullptr, "The fileName must not be null");

	const auto& foundTexture = mTextures.find(fileName);
	if (foundTexture != mTextures.end())
	{
		return reinterpret_cast<size_t>(&foundTexture->first);
	}

	std::string str = fileName;
	std::wstring wStr(str.begin(), str.end());

	ScratchImage image;
	HR(DirectX::LoadFromDDSFile(wStr.c_str(), DDS_FLAGS_NONE, nullptr, image));

	ID3D11ShaderResourceView* texture = nullptr;
	HR(DirectX::CreateShaderResourceView(mDevice, image.GetImages()
		, image.GetImageCount(), image.GetMetadata(), &texture));

	mTextures.insert(std::make_pair(str, texture));

	return reinterpret_cast<size_t>(&mTextures.find(str)->first);
}

void CompileShader(const char* fileName, const char* entryPoint, const char* shaderModel, ID3DBlob** outShaderByteCode)
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

	const size_t toWSTRSize = MultiByteToWideChar(CP_ACP, 0, fileName, -1, nullptr, NULL);
	auto toWSTR = new wchar_t[toWSTRSize];

	MultiByteToWideChar(CP_ACP, 0, fileName, strlen(fileName) + 1, toWSTR, toWSTRSize);

	if (FAILED(D3DCompileFromFile(toWSTR, nullptr, nullptr, entryPoint, shaderModel,
		shaderFlags, 0, outShaderByteCode, &errorBlob)))
	{
		ASSERT(errorBlob == nullptr, static_cast<const char*>(errorBlob->GetBufferPointer()));
		RELEASE_COM(errorBlob);
	}

	RELEASE_ARRAY(toWSTR);
}