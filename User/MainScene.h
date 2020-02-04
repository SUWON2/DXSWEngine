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
	void UpdateUnrealCamera(const float deltaTime);

	void UpdateUnityCamera(const float deltaTime, const DirectX::XMFLOAT3& viewDirection);

private:
	Text* mFrameText = nullptr;

	Text* mCameraModeText = nullptr;

	Text* mViewDirectionText = nullptr;

	Model* mTargetBlock = nullptr;

	// true = unreal camera mode, false = unity camera mode
	bool mIsCameraModeUnreal = false;
};