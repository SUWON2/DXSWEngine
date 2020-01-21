#pragma once

#include <d3d11.h>
#include <memory>
#include <vector>
#include <unordered_map>

#include "Camera/Camera.h"
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

	// HACK: 함수명 다시 생각해 보자
	void SortMeshAndText();

	void Draw();

	void AddMesh(Mesh* mesh)
	{
		ASSERT(mesh != nullptr, "The mesh must not be null");
		mMeshes.push_back(mesh);
	}

	void AddText(Text* text)
	{
		ASSERT(text != nullptr, "The text must not be null");
		mTexts.push_back(text);
	}

	size_t AddMaterial(Material* material);

	inline Camera* GetCamera() const
	{
		return mCamera.get();
	}

private:
	ID3D11Device* mDevice = nullptr;

	ID3D11DeviceContext* mDeviceContext = nullptr;

	std::unique_ptr<Camera> mCamera = nullptr;

	std::vector<Mesh*> mMeshes;

	std::vector<Text*> mTexts;

	size_t mFontMaterialID = 0;

	// material id, material
 	std::unordered_map<size_t, std::unique_ptr<Material>> mMaterials;
};