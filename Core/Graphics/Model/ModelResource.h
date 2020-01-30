#pragma once

#include <d3d11.h>
#include <vector>
#include <string>
#include <unordered_map>

class ModelResource final
{
public:
	struct Mesh
	{
		ID3D11Buffer* VertexBuffer;
		UINT VertexSize;
		UINT VertexCount;
	};

public:
	ModelResource(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	ModelResource(const ModelResource&) = delete;

	ModelResource& operator=(const ModelResource&) = delete;

	~ModelResource();

	// return vertex buffer id
	size_t LoadVertexBuffer(const char* fileName);

	inline const std::string& GetResourceName(const size_t id) const
	{
		return *reinterpret_cast<std::string*>(id);
	}

	inline const std::vector<Mesh>& GetModelData(const size_t id) const
	{
		return mModelDatas.at(GetResourceName(id));
	}

private:
	ID3D11Device* mDevice = nullptr;

	ID3D11DeviceContext* mDeviceContext = nullptr;

	// obj file name, model data
	std::unordered_map<std::string, std::vector<Mesh>> mModelDatas;
};