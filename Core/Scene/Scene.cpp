#include "Scene.h"

Scene::Scene()
{
	mRenderer = std::make_unique<Renderer>();
}

Scene::~Scene()
{
}
