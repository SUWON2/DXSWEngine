#pragma once

#include <d3d11.h>
#include <vector>
#include <string>
#include <unordered_map>

#include "../../../Common/Define.h"

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
	ID LoadVertexBuffer(const char* fileName);

	const std::string& GetResourceName(const ID id) const;

	const std::vector<Mesh>& GetModelData(const ID id) const;

private:
	ID3D11Device* mDevice = nullptr;

	ID3D11DeviceContext* mDeviceContext = nullptr;

	// obj file name, model data
	std::unordered_map<std::string, std::vector<Mesh>> mModelDatas;
};