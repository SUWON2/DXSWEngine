#pragma once

#include "../Core/Scene/Scene.h"

class MainScene final : public Scene
{
public:
	MainScene() = default;

	MainScene(const MainScene&) = delete;

	MainScene& operator=(const MainScene&) = delete;

	virtual ~MainScene() = default;

	void Initialize() override;

	void Update(const float deltaTime) override;

private:
	void UpdateCamera(const float deltaTime);

private:
	Text* mFPS = nullptr;

	Text* mViewDirectionText = nullptr;

	bool mIsCameraModeUnity = false;
};