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

	Material* CreateMaterial(const char* vertexShaderName, const char* pixelShaderName);

	ModelFrame* CreateModelFrame(const char* fileName, const std::vector<Material*>& materials);

	Text* CreateText();

	void DrawSkyDome();

	void PrepareForDrawingModel(const bool bEarlyZRejction);

	void DrawAllModel(ID3D11ShaderResourceView** shadowMap);

	void DrawAllText();

	Camera* GetCamera() const;

	SkyDome* GetSkyDome() const;

private:
	void CreateFrustumPlanes(const DirectX::XMMATRIX& matViewProjection, std::array<DirectX::XMVECTOR, 6>* outPlanes);

	void CreateShaderAndBufferZOnlyPass();

private:
	ID3D11Device* mDevice = nullptr;

	ID3D11DeviceContext* mDeviceContext = nullptr;

	ID3D11VertexShader* mZPassShader = nullptr;
	
	ID3D11InputLayout* mZPassLayout = nullptr;

	ID3D11Buffer* mZPassConstantBuffer = nullptr;

	std::unique_ptr<Camera> mCamera = nullptr;

	std::vector<ModelFrame*> mModelFrames = {};

	std::vector<Text*> mTexts = {};

	std::unique_ptr<SkyDome> mSkyDome = nullptr;

	// material, vector<model frame, mesh index of model frame>
	std::unordered_map<const Material*, std::vector<std::pair<ModelFrame*, int>>> mMaterials = {};

	Material* mBasicFontMaterial = nullptr;

	DirectX::XMMATRIX mMatViewProjection;
};