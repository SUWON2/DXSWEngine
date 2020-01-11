#pragma once

#include <d3d11.h>
#include <unordered_map>

class MeshResource final
{
public:
	struct VertexBuffer
	{
		ID3D11Buffer* Interface;
		UINT Size;
		UINT VertexCount;
	};

public:
	MeshResource(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	MeshResource(const MeshResource&) = delete;

	MeshResource& operator=(const MeshResource&) = delete;

	~MeshResource();

	// return vertex buffer id
	size_t RegisterVertexBuffer(const char* fileName);

	inline const std::string& GetResourceName(const size_t id) const
	{
		return *reinterpret_cast<std::string*>(id);
	}

	inline const VertexBuffer& GetVertexBuffer(const size_t id) const
	{
		return mVertexBuffers.at(GetResourceName(id));
	}

private:
	ID3D11Device* mDevice = nullptr;

	ID3D11DeviceContext* mDeviceContext = nullptr;

	std::unordered_map<std::string, VertexBuffer> mVertexBuffers;
};