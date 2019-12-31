#include "Scene.h"

Scene::Scene()
{
	mReneder = std::make_unique<Reneder>();
}

Scene::~Scene()
{
}
