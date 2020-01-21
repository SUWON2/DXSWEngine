#include "Mesh.h"
#include "../../../Common/Define.h"

ID3D11Device* Mesh::mDevice = nullptr;
ID3D11DeviceContext* Mesh::mDeviceContext = nullptr;
std::unique_ptr<MeshResource> Mesh::mMeshResource = nullptr;

Mesh::Mesh(const char* fileName, const size_t materialID)
	: mMaterialID(materialID)
{
	ASSERT(fileName != nullptr, "The fileName must not be null");
	ASSERT(mMeshResource != nullptr, "The mMeshResource is null");

	mVertexBufferID = mMeshResource->LoadVertexBuffer(fileName);
}

Mesh::~Mesh()
{
}

void Mesh::Initialize(RendererKey, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	ASSERT(device != nullptr, "The device must not be null");
	ASSERT(deviceContext != nullptr, "The deviceContext must not be null");

	mDevice = device;
	mDeviceContext = deviceContext;
	mMeshResource = std::make_unique<MeshResource>(device, deviceContext);
}

void Mesh::Draw(RendererKey)
{
	const MeshResource::VertexBuffer& vertexBuffer = mMeshResource->GetVertexBuffer(mVertexBufferID);

	const UINT stride = vertexBuffer.VertexSize;
	const UINT offset = 0;
	mDeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer.Interface, &stride, &offset);

	mDeviceContext->Draw(vertexBuffer.VertexCount, 0);
}