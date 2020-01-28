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
	mVertexShaderID = mMaterialResource->LoadVertexShader(vertexShaderName);
	mPixelShaderID = mMaterialResource->LoadPixelShader(pixelShaderName);

	// ��� ���ؽ� ���̴��� world, viewProjection matrix�� �׻� ���ʷ� �ڵ� �����ǵ��� �����մϴ�. ���� ���� 0��°, 1��° ���ۿ� �����մϴ�.
	// world�� viewProjection�� ���� ������ ������Ʈ�Ǵ� �ñ��Դϴ�.
	RegisterBuffer(0, sizeof(XMMATRIX), nullptr); // register world
	RegisterBuffer(1, sizeof(XMMATRIX), nullptr); // register viewProjection
}

Material::~Material()
{
	for (auto& i : mConstantBuffers)
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
	ASSERT(mTextureIDs.find(textureIndex) == mTextureIDs.end(), "�̹� ���ǰ� �ִ� �ؽ�ó �ε����Դϴ�. �ٸ� �ε����� ����� �ּ���");

	const size_t textureID = mMaterialResource->LoadTexture(fileName);
	mTextureIDs.insert(std::make_pair(textureIndex, textureID));
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
	mDeviceContext->VSSetShader(mMaterialResource->GetVertexShaderBuffer(mVertexShaderID).Interface, nullptr, 0);
	mDeviceContext->IASetInputLayout(mMaterialResource->GetVertexShaderBuffer(mVertexShaderID).InputLayout);
	mDeviceContext->PSSetShader(mMaterialResource->GetPixelShader(mPixelShaderID), nullptr, 0);

	for (const auto& constantBuffer : mConstantBuffers)
	{
		mDeviceContext->VSSetConstantBuffers(constantBuffer.first, 1, &constantBuffer.second);
	}

	for (const auto& texture : mTextureIDs)
	{
		mDeviceContext->PSSetShaderResources(texture.first, 1, mMaterialResource->GetTexture(texture.second));
	}
}
