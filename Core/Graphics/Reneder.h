#pragma once

#include <d3d11.h>
#include <memory>
#include <vector>
#include <unordered_map>

#include "Camera/Camera.h"
#include "Mesh/Mesh.h"
#include "Material/Material.h"
#include "../../Common/Define.h"

class Reneder final
{
public:
	Reneder();

	Reneder(const Reneder&) = default;

	Reneder& operator=(const Reneder&) = delete;

	~Reneder();

	void InitializeManager(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	void SortMesh();

	void Draw();

	void AddMesh(Mesh* mesh);

	size_t AddMaterial(Material* material);

	inline Camera* GetCamera() const
	{
		return mCamera.get();
	}

private:
	ID3D11Device* mDevice = nullptr;

	ID3D11DeviceContext* mDeviceContext = nullptr;

	std::unique_ptr<Camera> mCamera = nullptr;

	// mesh id, mesh
	std::vector<Mesh*> mMeshes;

	// material id, material
 	std::unordered_map<size_t, std::unique_ptr<Material>> mMaterials;
};