#pragma once

#include <d3d11.h>
#include <memory>
#include <vector>
#include <unordered_map>

#include "Camera/Camera.h"
#include "SkyDome/SkyDome.h"
#include "Mesh/Mesh.h"
#include "Text/Text.h"
#include "Material/Material.h"
#include "../../Common/Define.h"

class Renderer final
{
public:
	Renderer();

	Renderer(const Renderer&) = default;

	Renderer& operator=(const Renderer&) = delete;

	~Renderer();

	void InitializeManager(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	void SortMeshAndText();

	void DrawSkyDome();

	void DrawMeshAndText();

	void AddMesh(Mesh* mesh);

	void AddText(Text* text);

	size_t AddMaterial(Material* material);

	inline Camera* GetCamera() const
	{
		return mCamera.get();
	}

	inline SkyDome* GetSkyDome() const
	{
		return mSkyDome.get();
	}

private:
	ID3D11Device* mDevice = nullptr;

	ID3D11DeviceContext* mDeviceContext = nullptr;

	std::unique_ptr<Camera> mCamera = nullptr;

	std::unique_ptr<SkyDome> mSkyDome = nullptr;

	std::vector<Mesh*> mMeshes;

	std::vector<Text*> mTexts;

	size_t mFontMaterialID = 0;

	// material id, material
 	std::unordered_map<size_t, std::unique_ptr<Material>> mMaterials;
};