#include "Model.h"
#include "../../../Common/Define.h"

ID3D11Device* Model::mDevice = nullptr;
ID3D11DeviceContext* Model::mDeviceContext = nullptr;
std::unique_ptr<ModelResource> Model::mModelResource = nullptr;

Model::Model(const char* fileName)
{
	ASSERT(fileName != nullptr, "The fileName must not be null");
	ASSERT(mModelResource != nullptr, "The mModelResource is null");

	mResourceID = mModelResource->LoadVertexBuffer(fileName);

	mMeshCount = mModelResource->GetModelData(mResourceID).size();
	mMaterialIDs = std::make_unique<size_t[]>(mMeshCount);
}

Model::~Model()
{
}

Model* Model::Create(const char* fileName)
{
	return new Model(fileName);
}

void Model::SetMaterial(const unsigned int materialIndex, const size_t materialID)
{
	// HACK: Ŀ�ǵ� �� ��Ȯ�ϰ� ��������.
	ASSERT(0 <= materialIndex && materialIndex <= mMeshCount, "��Ƽ������ model�� �����ϴ� �޽� �������� �� ���� ����� �� �����ϴ�.");
	mMaterialIDs[materialIndex] = materialID;
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
	const ModelResource::Mesh& mesh = mModelResource->GetModelData(mResourceID).at(meshIndex);

	const UINT stride = mesh.VertexSize;
	const UINT offset = 0;
	mDeviceContext->IASetVertexBuffers(0, 1, &mesh.VertexBuffer, &stride, &offset);

	mDeviceContext->Draw(mesh.VertexCount, 0);
}