#pragma once

#include "../Core/Scene/Scene.h"

class MainScene final : public Scene
{
public:
	MainScene();

	MainScene(const MainScene&) = delete;

	MainScene& operator=(const MainScene&) = delete;

	virtual ~MainScene();

	void Initialize() override;

	void Update(const float deltaTime) override;

private:
	void UpdateCamera(const float deltaTime);

private:
	Text* mFrameText = nullptr;
};