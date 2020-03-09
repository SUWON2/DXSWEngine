#pragma once

#include <memory>

#include "../Graphics/Renderer.h"
#include "../CoreKey.h"
#include "../../Common/Define.h"

class Scene abstract
{
public:
	Scene();

	Scene(const Scene&) = delete;

	Scene& operator=(const Scene&) = delete;

	virtual ~Scene() = default;

	virtual void Initialize() = 0;

	virtual void Update(const float deltaTime) = 0;

	Material* CreateMaterial(const char* vertexShaderName, const char* pixelShaderName) const;

	ModelFrame* CreateModelFrame(const char* fileName, const std::vector<Material*>& materials) const;

	Text* CreateText() const;
	
	Camera* GetCamera() const;

	SkyDome* GetSkyDome() const;

public:
	Renderer* _GetRenderer(CoreKey) const;

private:
	std::unique_ptr<Renderer> mRenderer = nullptr;
};