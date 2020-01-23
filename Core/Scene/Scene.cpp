#include "Scene.h"

Scene::Scene()
{
	mRenderer = std::make_unique<Renderer>();
}

Scene::~Scene()
{
}

void Scene::AddMesh(Mesh* mesh)
{
	ASSERT(mesh != nullptr, "The mesh must not be null");
	mRenderer->AddMesh(mesh);
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