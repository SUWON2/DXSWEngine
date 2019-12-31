#pragma once

#include <memory>
#include <d3d11.h>

class Mesh;

class MeshManager final
{
public:
	MeshManager(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	MeshManager(const MeshManager&) = delete;

	MeshManager& operator=(const MeshManager&) = delete;

	~MeshManager();

	void Draw();

	void AddMesh(Mesh* mesh);

private:
	ID3D11Device* mDevice = nullptr;

	ID3D11DeviceContext* mDeviceContext = nullptr;

	// HACK: 메시 테스트를 위해서 메시 자원 관리는 안하고 하나만 우선 처리함.
	std::unique_ptr<Mesh> mMesh = nullptr;
	ID3D11Buffer* mVertexBuffer = nullptr;
	int mVertexCount = 0;
};