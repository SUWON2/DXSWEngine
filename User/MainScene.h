#pragma once

#include "../Core/Scene.h"

class MainScene : public Scene
{
public:
	void Initialize() override;

	void Update(const float deltaTime) override;
};