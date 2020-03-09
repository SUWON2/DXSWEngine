#define INITGUID // D3DReflect()�� ����ϱ� ���� �����Ѵ�.

#include <d3dcompiler.h>
#include <memory>

#include "MaterialResource.h"

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
	for (auto& i : mTextureArrays)
	{
		i->Release();
	}

	for (auto& i : mSingleTextures)
	{
		RELEASE_COM(i.second.first);
		RELEASE_COM(i.second.second);
	}

	for (auto& i : mPixelShaders)
	{
		RELEASE_COM(i.second)
	}

	for (auto& i : mVertexShaderBuffers)
	{
		RELEASE_COM(i.second.first)
		RELEASE_COM(i.second.second)
	}
}

ID MaterialResource::LoadVertexShader(const char* fileName)
{
	ASSERT(fileName != nullptr, "The fileName msut not be null");

	// �̹� �����ϴ� ���ؽ� ���̴��� ���� ������ �ʰ� ������ �ִ� ���� ��ȯ�մϴ�.
	const auto& foundVertexShader = mVertexShaderBuffers.find(fileName);
	if (foundVertexShader != mVertexShaderBuffers.end())
	{
		return reinterpret_cast<ID>(&foundVertexShader->second);
	}

	ID3DBlob* vertexShaderByteCode = nullptr;
	CompileShader(fileName, "VS", "vs_4_0", &vertexShaderByteCode);

	ID3D11VertexShader* vertexShader = nullptr;
	HR(mDevice->CreateVertexShader(vertexShaderByteCode->GetBufferPointer(), vertexShaderByteCode->GetBufferSize()
		, nullptr, &vertexShader));

	// ID3D11ShaderReflection�� ���� �����ϵ� ���̴��� ������ ���� �ɴϴ�. �� �ڼ��� ������ �Ʒ� ��ũ�� ���� Ȯ���� �� �ֽ��ϴ�.
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
		// ���ؽ� ���̴��� �Է� �Ķ���͸� �����ɴϴ�.
		D3D11_SIGNATURE_PARAMETER_DESC parameterDesc;
		vertexShaderReflection->GetInputParameterDesc(i, &parameterDesc);

		D3D11_INPUT_ELEMENT_DESC& inputLayout = inputLayoutDescList[i];
		inputLayout.SemanticName = parameterDesc.SemanticName;
		inputLayout.SemanticIndex = parameterDesc.SemanticIndex;

		// ���̴� �ø�ƽ �̸��� ���ξ� I_�� ������ �ν��Ͻ� ���� �����ͷ� �����ϰ� �ƴ� ���� ���ؽ� ���� �����ͷ� �����մϴ�.
		if (parameterDesc.SemanticName[0] != 'I'
			|| parameterDesc.SemanticName[1] != '_')
		{
			inputLayout.InputSlot = 0;
			inputLayout.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			inputLayout.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			inputLayout.InstanceDataStepRate = 0;
		}
		else
		{
			const char* instanceParamterName = &parameterDesc.SemanticName[2];

			if (strcmp(instanceParamterName, "WORLD") == 0)
			{
				inputLayout.InputSlot = 1;
			}
			else if (strcmp(instanceParamterName, "TEXTURE_INDEX") == 0)
			{
				inputLayout.InputSlot = 2;
			}
			else
			{
				ASSERT(false, "�������� �ʴ� �ø�ƽ �̸��Դϴ�.");
			}

			inputLayout.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			inputLayout.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
			inputLayout.InstanceDataStepRate = 1;
		}

		// D3D ������ DXGI �������� ������ŵ�ϴ�.
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

	mVertexShaderBuffers.insert({ fileName, { vertexShader, inputLayout } });

	return reinterpret_cast<ID>(&mVertexShaderBuffers.find(fileName)->second);
}

ID MaterialResource::LoadPixelShader(const char* fileName)
{
	ASSERT(fileName != nullptr, "The fileName msut not be null");

	// �̹� �����ϴ� �ȼ� ���̴��� ���� ������ �ʰ� ������ �ִ� ���� ��ȯ�մϴ�.
	const auto& foundPixelShader = mPixelShaders.find(fileName);
	if (foundPixelShader != mPixelShaders.end())
	{
		return reinterpret_cast<ID>(foundPixelShader->second);
	}

	ID3DBlob* pixelShaderByteCode = nullptr;
	CompileShader(fileName, "PS", "ps_4_0", &pixelShaderByteCode);

	ID3D11PixelShader* pixelShader = nullptr;
	HR(mDevice->CreatePixelShader(pixelShaderByteCode->GetBufferPointer()
		, pixelShaderByteCode->GetBufferSize(), nullptr, &pixelShader));

	RELEASE_COM(pixelShaderByteCode);

	mPixelShaders.insert({ fileName, pixelShader });

	return reinterpret_cast<ID>(pixelShader);
}

ID MaterialResource::LoadSingleTexture(const std::string& fileName)
{
	// �̹� �����ϴ� �ؽ�ó�� ���� ������ �ʰ� ������ �ִ� ���� ��ȯ�մϴ�.
	const auto& foundTexture = mSingleTextures.find(fileName);
	if (foundTexture != mSingleTextures.end())
	{
		return reinterpret_cast<ID>(foundTexture->second.second);
	}

	ScratchImage image;
	HR(LoadFromDDSFile(std::wstring(fileName.begin(), fileName.end()).c_str()
		, DDS_FLAGS_NONE, nullptr, image));

	ID3D11Texture2D* texture;
	HR(CreateTexture(mDevice, image.GetImage(0, 0, 0), image.GetImageCount(), image.GetMetadata()
		, reinterpret_cast<ID3D11Resource**>(&texture)));

	ID3D11ShaderResourceView* srvTexture;
	HR(mDevice->CreateShaderResourceView(texture, nullptr, &srvTexture));

	mSingleTextures.insert({ fileName, { texture, srvTexture} });

	image.Release();

	return reinterpret_cast<ID>(srvTexture);
}

ID MaterialResource::LoadTextureArray(const XMINT2& size, const std::vector<std::string>& fileNames)
{
	D3D11_TEXTURE2D_DESC textureArrayDesc = {};
	textureArrayDesc.Width = size.x;
	textureArrayDesc.Height = size.y;
	textureArrayDesc.MipLevels = 1;
	textureArrayDesc.ArraySize = fileNames.size();
	textureArrayDesc.Format = DXGI_FORMAT_BC7_UNORM;
	textureArrayDesc.SampleDesc.Count = 1;
	textureArrayDesc.SampleDesc.Quality = 0;
	textureArrayDesc.Usage = D3D11_USAGE_DEFAULT;
	textureArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureArrayDesc.CPUAccessFlags = 0;
	textureArrayDesc.MiscFlags = 0;

	ID3D11Texture2D* textureArray;
	HR(mDevice->CreateTexture2D(&textureArrayDesc, nullptr, &textureArray));

	// �ؽ�ó�� ���� ����ų� �̹� �����ϴ� �ؽ�ó�� ���� ������ �ʰ� ������ �ִ� ���� ����Ͽ� �ؽ�ó ��̿� �����մϴ�.
	for (size_t i = 0; i < fileNames.size(); ++i)
	{
		ID3D11Texture2D* texture;
		
		const auto& foundTexture = mSingleTextures.find(fileNames[i]);

		if (foundTexture != mSingleTextures.end())
		{
			#if defined(DEBUG) | defined(_DEBUG)
			ScratchImage image;
			HR(LoadFromDDSFile(std::wstring(fileNames[i].begin(), fileNames[i].end()).c_str()
				, DDS_FLAGS_NONE, nullptr, image));

			ASSERT(textureArrayDesc.Width == image.GetMetadata().width
				&& textureArrayDesc.Height == image.GetMetadata().height,
				"�ؽ�ó ũ�Ⱑ �ؽ�ó ����� ũ��� ��ġ���� �ʽ��ϴ�.");

			ASSERT(textureArrayDesc.Format == image.GetMetadata().format,
				"�ؽ�ó ������ �ؽ�ó ����� ���˰� ��ġ���� �ʽ��ϴ�.");

			image.Release();
			#endif

			texture = foundTexture->second.first;
		}
		else
		{
			ScratchImage image;
			HR(LoadFromDDSFile(std::wstring(fileNames[i].begin(), fileNames[i].end()).c_str()
				, DDS_FLAGS_NONE, nullptr, image));

			ASSERT(textureArrayDesc.Width == image.GetMetadata().width
				&& textureArrayDesc.Height == image.GetMetadata().height,
				"�ؽ�ó ũ�Ⱑ �ؽ�ó ����� ũ��� ��ġ���� �ʽ��ϴ�.");

			ASSERT(textureArrayDesc.Format == image.GetMetadata().format,
				"�ؽ�ó ������ �ؽ�ó ����� ���˰� ��ġ���� �ʽ��ϴ�.");

			HR(CreateTexture(mDevice, image.GetImage(0, 0, 0), image.GetImageCount(), image.GetMetadata()
				, reinterpret_cast<ID3D11Resource**>(&texture)));

			ID3D11ShaderResourceView* srvTexture;
			HR(mDevice->CreateShaderResourceView(texture, nullptr, &srvTexture));

			mSingleTextures.insert({ fileNames[i], { texture, srvTexture } });

			image.Release();
		}

		mDeviceContext->CopySubresourceRegion(textureArray, D3D11CalcSubresource(0, i, textureArrayDesc.MipLevels), 0, 0, 0, texture, 0, nullptr);
	}

	ID3D11ShaderResourceView* srvTextureArray;
	HR(mDevice->CreateShaderResourceView(textureArray, nullptr, &srvTextureArray));

	mTextureArrays.insert(srvTextureArray);

	RELEASE_COM(textureArray);

	return reinterpret_cast<ID>(srvTextureArray);
}

const std::string& MaterialResource::GetResourceName(const ID id) const
{
	return *reinterpret_cast<std::string*>(id);
}

const std::pair<ID3D11VertexShader*, ID3D11InputLayout*>& MaterialResource::GetVertexShaderBuffer(const ID id) const
{
	return *reinterpret_cast<std::pair<ID3D11VertexShader*, ID3D11InputLayout*>*>(id);
}

ID3D11PixelShader* MaterialResource::GetPixelShader(const ID id) const
{
	return reinterpret_cast<ID3D11PixelShader*>(id);
}

ID3D11ShaderResourceView* MaterialResource::GetSrvTexture(const ID id) const
{
	return reinterpret_cast<ID3D11ShaderResourceView*>(id);
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

	const int toWSTRSize = MultiByteToWideChar(CP_ACP, 0, fileName, -1, nullptr, NULL);
	wchar_t* toWSTR = new wchar_t[toWSTRSize];

	MultiByteToWideChar(CP_ACP, 0, fileName, static_cast<int>(strlen(fileName)) + 1, toWSTR, toWSTRSize);

	if (FAILED(D3DCompileFromFile(toWSTR, nullptr, nullptr, entryPoint, shaderModel,
		shaderFlags, 0, outShaderByteCode, &errorBlob)))
	{
		ASSERT(errorBlob == nullptr, static_cast<const char*>(errorBlob->GetBufferPointer()));
		RELEASE_COM(errorBlob);
	}

	RELEASE_ARRAY(toWSTR);
}