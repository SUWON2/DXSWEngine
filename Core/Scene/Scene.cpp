#include "Scene.h"

Scene::Scene()
{
	mRenderer = std::make_unique<Renderer>();
}

Scene::~Scene()
{
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

size_t Scene::AddMaterial(Material* material)
{
	ASSERT(material != nullptr, "The material must not be null");
	return mRenderer->AddMaterial(material);
}