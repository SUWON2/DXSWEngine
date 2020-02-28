#include "Scene.h"

Scene::Scene()
{
	mRenderer = std::make_unique<Renderer>();
}

void Scene::AddModel(Model* model)
{
	ASSERT(model != nullptr, "The model must not be null");
	mRenderer->AddModel(model);
}

void Scene::AddText(Text* text)
{
	ASSERT(text != nullptr, "The text must not be null");
	mRenderer->AddText(text);
}

ID Scene::AddMaterial(Material* material)
{
	ASSERT(material != nullptr, "The material must not be null");
	return mRenderer->AddMaterial(material);
}

Camera* Scene::GetCamera() const
{
	return mRenderer->GetCamera();
}

SkyDome* Scene::GetSkyDome() const
{
	return mRenderer->GetSkyDome();
}

size_t Scene::GetModelCount() const
{
	return mRenderer->GetModelCount();
}

Renderer* Scene::_GetRenderer(CoreKey) const
{
	return mRenderer.get();
}
