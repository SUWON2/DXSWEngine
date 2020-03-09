#include "Scene.h"

Scene::Scene()
{
	mRenderer = std::make_unique<Renderer>();
}

Material* Scene::CreateMaterial(const char* vertexShaderName, const char* pixelShaderName) const
{
	return mRenderer->CreateMaterial(vertexShaderName, pixelShaderName);
}

ModelFrame* Scene::CreateModelFrame(const char* fileName, const std::vector<Material*>& materials) const
{
	return mRenderer->CreateModelFrame(fileName, materials);
}

Text* Scene::CreateText() const
{
	return mRenderer->CreateText();
}

Camera* Scene::GetCamera() const
{
	return mRenderer->GetCamera();
}

SkyDome* Scene::GetSkyDome() const
{
	return mRenderer->GetSkyDome();
}

Renderer* Scene::_GetRenderer(CoreKey) const
{
	return mRenderer.get();
}
