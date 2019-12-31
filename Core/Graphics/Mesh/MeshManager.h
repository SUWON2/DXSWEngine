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

	// HACK: �޽� �׽�Ʈ�� ���ؼ� �޽� �ڿ� ������ ���ϰ� �ϳ��� �켱 ó����.
	std::unique_ptr<Mesh> mMesh = nullptr;
	ID3D11Buffer* mVertexBuffer = nullptr;
	int mVertexCount = 0;
};