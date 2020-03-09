#pragma once

#include <d3d11.h>
#include <memory>
#include <vector>
#include <unordered_map>

#include "Camera/Camera.h"
#include "Material/Material.h"
#include "Model/ModelFrame.h"
#include "Text/Text.h"
#include "SkyDome/SkyDome.h"
#include "../../Common/Define.h"

class Renderer final
{
public:
	Renderer() = default;

	Renderer(const Renderer&) = default;

	Renderer& operator=(const Renderer&) = delete;

	~Renderer();

	void InitializeManager(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	void SortText();

	Material* CreateMaterial(const char* vertexShaderName, const char* pixelShaderName);

	ModelFrame* CreateModelFrame(const char* fileName, const std::vector<Material*>& materials);

	Text* CreateText();

	void DrawSkyDome();

	void DrawAllModel(ID3D11ShaderResourceView** shadowMap);

	void DrawAllText();

	Camera* GetCamera() const;

	SkyDome* GetSkyDome() const;

private:
	void CreateFrustumPlanes(const DirectX::XMMATRIX& matViewProjection, std::array<DirectX::XMVECTOR, 6>* outPlanes);

private:
	ID3D11Device* mDevice = nullptr;

	ID3D11DeviceContext* mDeviceContext = nullptr;

	std::unique_ptr<Camera> mCamera = nullptr;

	std::unique_ptr<SkyDome> mSkyDome = nullptr;

	std::vector<ModelFrame*> mModelFrames = {};

	std::vector<Text*> mTexts = {};

	// material, vector<model frame, mesh index of model frame>
	std::unordered_map<const Material*, std::vector<std::pair<ModelFrame*, int>>> mMaterials = {};

	Material* mBasicFontMaterial = nullptr;

	// HACK: Shadow
	Material* mShadowMaterial = nullptr;
};