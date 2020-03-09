#include "Material.h"

using namespace DirectX;

ID3D11Device* Material::mDevice = nullptr;
ID3D11DeviceContext* Material::mDeviceContext = nullptr;
std::unique_ptr<MaterialResource> Material::mMaterialResource = nullptr;

Material::Material(RendererKey, const char* vertexShaderName, const char* pixelShaderName, const bool bUseShadowMap)
{
	ASSERT(vertexShaderName != nullptr, "The vertexShaderName must not be null");
	ASSERT(pixelShaderName != nullptr, "The pixelShaderName must not be null");
	ASSERT(mMaterialResource != nullptr, "The mMaterialResourceManager must not be null");

	// 셰이더를 등록합니다.
	mVertexShaderId = mMaterialResource->LoadVertexShader(vertexShaderName);
	mPixelShaderId = mMaterialResource->LoadPixelShader(pixelShaderName);

	/*
		머티리얼이 생성될 때 world, viewProjection matrix를 항상 첫 번째와 두 번째 상수 버퍼에 지정합니다.
		따라서 모든 버텍스 셰이더에서는 항상 이 두 행렬을 받을 수 있도록 상수 버퍼를 만들어 놓아야 합니다.
	*/
	RegisterBuffer<ShaderType::VS>(0, sizeof(XMMATRIX), nullptr); // register world
	RegisterBuffer<ShaderType::VS>(1, sizeof(XMMATRIX), nullptr); // register viewProjection

	mbUseShadowMap = bUseShadowMap;

	// TODO: 주석달자
	// mbUseShadowMap이 true일 때 버텍스 1인덱스에 한 번에 패스할 수 있도록 처리하자.
	if (mbUseShadowMap)
	{
		RegisterBuffer<ShaderType::VS>(2, sizeof(XMMATRIX), nullptr);
	}
}

Material::~Material()
{
	for (auto& i : mPSConstantBuffers)
	{
		RELEASE_COM(i.second);
	}

	for (auto& i : mVSConstantBuffers)
	{
		RELEASE_COM(i.second);
	}
}

void Material::RegisterTexture(const unsigned int index, const char* fileName)
{
	ASSERT(!mbUseShadowMap || (mbUseShadowMap && index > 0)
		, "쉐도우 맵을 사용한다고 지정한 경우 픽셀 셰이더의 텍스처 인덱스 0번째는 엔진 내부에서 ShadowMap으로 사용되니 1 이상의 텍스처 인덱스로만 등록해 주세요");
	ASSERT(fileName != nullptr, "The fileName must not be null");
	ASSERT(mSingleTextureIds.find(index) == mSingleTextureIds.end() && mTextureArrayIds.find(index) == mTextureArrayIds.end()
		, "이미 사용되고 있는 텍스처 인덱스입니다. 다른 인덱스로 등록해 주세요");

	mSingleTextureIds.insert({ index, mMaterialResource->LoadSingleTexture(fileName) });
}

void Material::RegisterTextureArray(const unsigned int index, const XMINT2& size, const std::vector<std::string>& fileNames)
{
	ASSERT(mSingleTextureIds.find(index) == mSingleTextureIds.end() && mTextureArrayIds.find(index) == mTextureArrayIds.end()
		, "이미 사용되고 있는 텍스처 인덱스입니다. 다른 인덱스로 등록해 주세요");

	mTextureArrayIds.insert({ index, mMaterialResource->LoadTextureArray(size, fileNames) });
}

const char* Material::GetVertexShaderName() const
{
	return mMaterialResource->GetResourceName(mVertexShaderId).c_str();
}

const char* Material::GetPixelShaderName() const
{
	return mMaterialResource->GetResourceName(mPixelShaderId).c_str();
}

void Material::_Initialize(RendererKey, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	ASSERT(device != nullptr, "The device must not be null");
	ASSERT(deviceContext != nullptr, "The deviceContext must not be null");

	mDevice = device;
	mDeviceContext = deviceContext;
	mMaterialResource = std::make_unique<MaterialResource>(device, deviceContext);
}

void Material::_Active(RendererKey, ID3D11ShaderResourceView** shadowMap, const XMMATRIX& matViewProjection) const
{
	ASSERT(!mbUseShadowMap || (mbUseShadowMap && shadowMap != nullptr), "The shoadwMap must not be null if mbUseShadow is true");

	const std::pair<ID3D11VertexShader*, ID3D11InputLayout*>& vertexShaderBuffer
		= mMaterialResource->GetVertexShaderBuffer(mVertexShaderId);

	mDeviceContext->VSSetShader(vertexShaderBuffer.first, nullptr, 0);
	mDeviceContext->IASetInputLayout(vertexShaderBuffer.second);

	mDeviceContext->PSSetShader(mMaterialResource->GetPixelShader(mPixelShaderId), nullptr, 0);

	for (const auto& vsConstantBuffer : mVSConstantBuffers)
	{
		mDeviceContext->VSSetConstantBuffers(vsConstantBuffer.first, 1, &vsConstantBuffer.second);
	}

	for (const auto& psConstantBuffer : mPSConstantBuffers)
	{
		mDeviceContext->PSSetConstantBuffers(psConstantBuffer.first, 1, &psConstantBuffer.second);
	}

	if (mbUseShadowMap)
	{
		/*
			쉐도우 맵을 사용한다면 항상 픽셀 셰이더의 첫 번째 텍스처에 지정됩니다.
			따라서 쉐도우 맵을 사용하는 픽셀 셰이더에서는 쉐도우 맵을 받을 수 있도록 텍스처를 첫 번째 인덱스로 만들어 놓아야 합니다.
		*/
		mDeviceContext->PSSetShaderResources(0, 1, shadowMap);
	}

	ID3D11ShaderResourceView* srvTexture = nullptr;

	for (const auto& texture : mSingleTextureIds)
	{
		srvTexture = mMaterialResource->GetSrvTexture(texture.second);
		mDeviceContext->PSSetShaderResources(texture.first, 1, &srvTexture);
	}

	for (const auto& textureArray : mTextureArrayIds)
	{
		srvTexture = mMaterialResource->GetSrvTexture(textureArray.second);
		mDeviceContext->PSSetShaderResources(textureArray.first, 1, &srvTexture);
	}
	
	UpdateBuffer<Material::ShaderType::VS>(1, XMMatrixTranspose(matViewProjection));
}

void Material::_ActiveWithShadowMode(RendererKey)
{
	const std::pair<ID3D11VertexShader*, ID3D11InputLayout*>& vertexShaderBuffer 
		= mMaterialResource->GetVertexShaderBuffer(mVertexShaderId);

	mDeviceContext->VSSetShader(vertexShaderBuffer.first, nullptr, 0);
	mDeviceContext->IASetInputLayout(vertexShaderBuffer.second);

	mDeviceContext->PSSetShader(mMaterialResource->GetPixelShader(mPixelShaderId), nullptr, 0);

	for (const auto& vsConstantBuffer : mVSConstantBuffers)
	{
		mDeviceContext->VSSetConstantBuffers(vsConstantBuffer.first, 1, &vsConstantBuffer.second);
	}
}

bool Material::_IsUsedShadowMap() const
{
	return mbUseShadowMap;
}
