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

	// ���̴��� ����մϴ�.
	mVertexShaderId = mMaterialResource->LoadVertexShader(vertexShaderName);
	mPixelShaderId = mMaterialResource->LoadPixelShader(pixelShaderName);

	/*
		��Ƽ������ ������ �� world, viewProjection matrix�� �׻� ù ��°�� �� ��° ��� ���ۿ� �����մϴ�.
		���� ��� ���ؽ� ���̴������� �׻� �� �� ����� ���� �� �ֵ��� ��� ���۸� ����� ���ƾ� �մϴ�.
	*/
	RegisterBuffer<ShaderType::VS>(0, sizeof(XMMATRIX), nullptr); // register world
	RegisterBuffer<ShaderType::VS>(1, sizeof(XMMATRIX), nullptr); // register viewProjection

	mbUseShadowMap = bUseShadowMap;

	// TODO: �ּ�����
	// mbUseShadowMap�� true�� �� ���ؽ� 1�ε����� �� ���� �н��� �� �ֵ��� ó������.
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
		, "������ ���� ����Ѵٰ� ������ ��� �ȼ� ���̴��� �ؽ�ó �ε��� 0��°�� ���� ���ο��� ShadowMap���� ���Ǵ� 1 �̻��� �ؽ�ó �ε����θ� ����� �ּ���");
	ASSERT(fileName != nullptr, "The fileName must not be null");
	ASSERT(mSingleTextureIds.find(index) == mSingleTextureIds.end() && mTextureArrayIds.find(index) == mTextureArrayIds.end()
		, "�̹� ���ǰ� �ִ� �ؽ�ó �ε����Դϴ�. �ٸ� �ε����� ����� �ּ���");

	mSingleTextureIds.insert({ index, mMaterialResource->LoadSingleTexture(fileName) });
}

void Material::RegisterTextureArray(const unsigned int index, const XMINT2& size, const std::vector<std::string>& fileNames)
{
	ASSERT(mSingleTextureIds.find(index) == mSingleTextureIds.end() && mTextureArrayIds.find(index) == mTextureArrayIds.end()
		, "�̹� ���ǰ� �ִ� �ؽ�ó �ε����Դϴ�. �ٸ� �ε����� ����� �ּ���");

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
			������ ���� ����Ѵٸ� �׻� �ȼ� ���̴��� ù ��° �ؽ�ó�� �����˴ϴ�.
			���� ������ ���� ����ϴ� �ȼ� ���̴������� ������ ���� ���� �� �ֵ��� �ؽ�ó�� ù ��° �ε����� ����� ���ƾ� �մϴ�.
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
