#pragma once

#include <d3d11.h>
#include <vector>
#include <string>
#include <unordered_map>

#include "../RendererKey.h"
#include "../../../Common/Define.h"
#include "../../../Common/DirectXMath.h"

class ModelResource final
{
public:
	struct Mesh
	{
		ID3D11Buffer* VertexBuffer;
		ID3D11Buffer* IndexBuffer;
		UINT IndexCount;
	};

	struct Vertex
	{
		DirectX::XMFLOAT4 Position;
		DirectX::XMFLOAT2 UV;
		DirectX::XMFLOAT3 Normal;
	};

public:
	ModelResource() = default;

	ModelResource(const ModelResource&) = delete;

	ModelResource& operator=(const ModelResource&) = delete;

	~ModelResource();

	void Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	ID LoadVertexBuffer(const char* fileName);

	const std::string& GetResourceName(const ID id) const;

	const std::vector<Mesh>& GetModelData(const ID id) const;

private:
	ID3D11Device* mDevice = nullptr;

	ID3D11DeviceContext* mDeviceContext = nullptr;

	// obj file name, model data
	std::unordered_map<std::string, std::vector<Mesh>> mModelDatas;
};