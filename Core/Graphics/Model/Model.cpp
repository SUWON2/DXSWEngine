#include "Model.h"
#include "../../../Common/Define.h"

ID3D11Device* Model::mDevice = nullptr;
ID3D11DeviceContext* Model::mDeviceContext = nullptr;
std::unique_ptr<ModelResource> Model::mModelResource = nullptr;

Model::Model(const char* fileName)
{
	ASSERT(fileName != nullptr, "The fileName must not be null");
	ASSERT(mModelResource != nullptr, "The mModelResource is null");

	mDataId = mModelResource->LoadVertexBuffer(fileName);

	mMeshCount = mModelResource->GetModelData(mDataId).size();
	mMaterialIds = std::make_unique<ID[]>(mMeshCount);
}

Model* Model::Create(const char* fileName)
{
	return new Model(fileName);
}

const char* Model::GetFileName() const
{
	return mModelResource->GetResourceName(mDataId).c_str();
}

const std::unique_ptr<ID[]>& Model::GetMaterialIds() const
{
	return mMaterialIds;
}

size_t Model::GetMeshCount() const
{
	return mMeshCount;
}

bool Model::IsActive() const
{
	return mbActive;
}

const DirectX::XMFLOAT3& Model::GetPosition() const
{
	return mPosition;
}

const DirectX::XMFLOAT3& Model::GetScale() const
{
	return mScale;
}

const DirectX::XMFLOAT3& Model::GetRotation() const
{
	return mRotation;
}

void Model::SetMaterial(const unsigned int materialIndex, const ID materialId)
{
	ASSERT(0 <= materialIndex && materialIndex <= mMeshCount, "model이 보유하는 메쉬 개수보다 더 큰 머티리얼 인덱스를 등록할 수 없습니다.");
	mMaterialIds[materialIndex] = materialId;
}

void Model::SetActive(const bool bActive)
{
	mbActive = bActive;
}

void Model::SetPosition(const DirectX::XMFLOAT3& position)
{
	mPosition = position;
}

void Model::SetScale(const DirectX::XMFLOAT3& scale)
{
	mScale = scale;
}

void Model::SetRotation(const DirectX::XMFLOAT3& rotation)
{
	mRotation = rotation;
}

void Model::_Initialize(RendererKey, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	ASSERT(device != nullptr, "The device must not be null");
	ASSERT(deviceContext != nullptr, "The deviceContext must not be null");

	mDevice = device;
	mDeviceContext = deviceContext;
	mModelResource = std::make_unique<ModelResource>(device, deviceContext);
}

void Model::_Draw(RendererKey, const unsigned int meshIndex)
{
	const ModelResource::Mesh& mesh = mModelResource->GetModelData(mDataId).at(meshIndex);

	const UINT stride = mesh.VertexSize;
	const UINT offset = 0;
	mDeviceContext->IASetVertexBuffers(0, 1, &mesh.VertexBuffer, &stride, &offset);

	mDeviceContext->Draw(mesh.VertexCount, 0);
}