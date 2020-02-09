#include "Material.h"
#include "../../../Common/DirectXMath.h"

using namespace DirectX;

ID3D11Device* Material::mDevice = nullptr;
ID3D11DeviceContext* Material::mDeviceContext = nullptr;
std::unique_ptr<MaterialResource> Material::mMaterialResource = nullptr;

Material::Material(const char* vertexShaderName, const char* pixelShaderName)
{
	ASSERT(mMaterialResource != nullptr, "The mMaterialResourceManager must not be null");

	// ���̴��� ����մϴ�.
	mVertexShaderId = mMaterialResource->LoadVertexShader(vertexShaderName);
	mPixelShaderId = mMaterialResource->LoadPixelShader(pixelShaderName);

	// ��� ���ؽ� ���̴��� world, viewProjection matrix�� �׻� ���ʷ� �ڵ� �����ǵ��� �����մϴ�. ���� ���� 0��°, 1��° ���ۿ� �����մϴ�.
	// world�� viewProjection�� ���� ������ ������Ʈ�Ǵ� �ñ��Դϴ�.
	RegisterBuffer(ShaderType::VS, 0, sizeof(XMMATRIX), nullptr); // register world
	RegisterBuffer(ShaderType::VS, 1, sizeof(XMMATRIX), nullptr); // register viewProjection
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

Material* Material::Create(const char* vertexShaderName, const char* pixelShaderName)
{
	ASSERT(vertexShaderName != nullptr, "The vertexShaderName must not be null");
	ASSERT(pixelShaderName != nullptr, "The pixelShaderName must not be null");

	return new Material(vertexShaderName, pixelShaderName);
}

void Material::RegisterTexture(const unsigned int textureIndex, const char* fileName)
{
	ASSERT(fileName != nullptr, "The fileName must not be null");
	ASSERT(mTextureIds.find(textureIndex) == mTextureIds.end(), "�̹� ���ǰ� �ִ� �ؽ�ó �ε����Դϴ�. �ٸ� �ε����� ����� �ּ���");

	const ID textureId = mMaterialResource->LoadTexture(fileName);
	mTextureIds.insert(std::make_pair(textureIndex, textureId));
}

const char* Material::GetVertexShaderName() const
{
	return mMaterialResource->GetResourceName(mVertexShaderId).c_str();
}

const char* Material::GetPixelShaderName() const
{
	return mMaterialResource->GetResourceName(mPixelShaderId).c_str();
}

const char* Material::GetTextureName(const unsigned int index) const
{
	return mMaterialResource->GetResourceName(mTextureIds.at(index)).c_str();
}

void Material::_Initialize(RendererKey, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	ASSERT(device != nullptr, "The device must not be null");
	ASSERT(deviceContext != nullptr, "The deviceContext must not be null");

	mDevice = device;
	mDeviceContext = deviceContext;
	mMaterialResource = std::make_unique<MaterialResource>(device, deviceContext);
}

void Material::_Activate(RendererKey)
{
	mDeviceContext->VSSetShader(mMaterialResource->GetVertexShaderBuffer(mVertexShaderId).Interface, nullptr, 0);
	mDeviceContext->IASetInputLayout(mMaterialResource->GetVertexShaderBuffer(mVertexShaderId).InputLayout);
	mDeviceContext->PSSetShader(mMaterialResource->GetPixelShader(mPixelShaderId), nullptr, 0);

	for (const auto& vsConstantBuffer : mVSConstantBuffers)
	{
		mDeviceContext->VSSetConstantBuffers(vsConstantBuffer.first, 1, &vsConstantBuffer.second);
	}

	for (const auto& psConstantBuffer : mPSConstantBuffers)
	{
		mDeviceContext->PSSetConstantBuffers(psConstantBuffer.first, 1, &psConstantBuffer.second);
	}

	for (const auto& texture : mTextureIds)
	{
		mDeviceContext->PSSetShaderResources(texture.first, 1, mMaterialResource->GetTexture(texture.second));
	}
}
