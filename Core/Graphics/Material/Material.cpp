#include "Material.h"
#include "../../../Common/DirectXMath.h"

using namespace DirectX;

ID3D11Device* Material::mDevice = nullptr;
ID3D11DeviceContext* Material::mDeviceContext = nullptr;
std::unique_ptr<MaterialResource> Material::mMaterialResource = nullptr;

Material::Material(const char* vertexShaderName, const char* pixelShaderName)
{
	ASSERT(mMaterialResource != nullptr, "The mMaterialResourceManager must not be null");

	// 셰이더를 등록합니다.
	mVertexShaderID = mMaterialResource->RegisterVertexShader(vertexShaderName);
	mPixelShaderID = mMaterialResource->RegisterPixelShader(pixelShaderName);

	// 모든 버텍스 셰이더의 world, viewProjection matrix는 항상 최초로 자동 설정되도록 가정합니다. 따라서 각각 0번째, 1번째 버퍼에 설정합니다.
	// world와 viewProjection를 나눈 기준은 업데이트되는 시기입니다.
	RegisterBuffer(0, sizeof(XMMATRIX), nullptr); // register world
	RegisterBuffer(1, sizeof(XMMATRIX), nullptr); // register viewProjection
}

void Material::Initialize(RenederKey, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	ASSERT(device != nullptr, "The device must not be null");
	ASSERT(deviceContext != nullptr, "The deviceContext must not be null");

	mDevice = device;
	mDeviceContext = deviceContext;
	mMaterialResource = std::make_unique<MaterialResource>(device, deviceContext);
}

void Material::Activate(RenederKey)
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

Material::~Material()
{
	for (auto& i : mConstantBuffers)
	{
		RELEASE_COM(i.second);
	}
}

void Material::RegisterTexture(const unsigned int textureIndex, const char* fileName)
{
	ASSERT(fileName != nullptr, "The fileName must not be null");
	ASSERT(mTextureIDs.find(textureIndex) == mTextureIDs.end(), "이미 사용되고 있는 텍스처 인덱스입니다. 다른 인덱스로 등록해 주세요");

	const size_t textureID = mMaterialResource->RegisterTexture(fileName);
	mTextureIDs.insert(std::make_pair(textureIndex, textureID));
}
