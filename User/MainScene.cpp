#include "MainScene.h"

#include "../Core/Graphics/Mesh/Mesh.h"

MainScene::MainScene()
{
}

MainScene::~MainScene()
{
}

void MainScene::Initialize()
{
	Mesh* cube = Mesh::Create("Resource/Wolf.obj");
	AddMesh(cube);
}

void MainScene::Update(const float deltaTime)
{
}
