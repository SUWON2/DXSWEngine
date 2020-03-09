#pragma once

#include <array>
#include <memory>
#include <vector>

#include "Model.h"
#include "ModelResource.h"
#include "../RendererKey.h"
#include "../Material/Material.h"
#include "../../../Common/DirectXMath.h"

class ModelFrame final
{
private:
	struct ModelInstanceData
	{
		DirectX::XMFLOAT4X4 World;
	};

	struct MeshInstanceData
	{
		int textureIndex;
	};

public:
	ModelFrame(RendererKey, const char* fileName, const std::vector<Material*>& materials);

	ModelFrame(const ModelFrame&) = delete;

	ModelFrame& operator=(const ModelFrame&) = delete;

	~ModelFrame();

	void Create(Model** outModel);

	void Create(const int count, std::vector<Model*>* outModels);

	Material** GetMaterials() const;

	size_t GetMaterialCount() const;

public:
	static void _Initialize(RendererKey, ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	void _UpdateInstanceBuffer(RendererKey, const std::array<DirectX::XMVECTOR, 6>& planes);

	void _DrawMesh(RendererKey, const int index);

private:
	void RemakeInstanceBuffer();

private:
	static ID3D11Device* mDevice;

	static ID3D11DeviceContext* mDeviceContext;

	static std::unique_ptr<ModelResource> mModelResource;

	std::vector<Model*> mModels = {};

	std::pair<std::unique_ptr<Material * []>, size_t> mMaterials = {};

	std::pair<std::unique_ptr<ID3D11Buffer * []>, size_t> mInstanceBuffers = {};

	std::unique_ptr<D3D11_MAPPED_SUBRESOURCE[]> mMappedSubResource = nullptr;

	ID mModelId = 0;

	int mDrawnCount = 0;
};