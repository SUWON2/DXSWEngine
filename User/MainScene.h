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

	bool IsBoxCollidedByRaycast();

private:
	static constexpr DirectX::XMFLOAT3 PLAYER_START_POSITION = { 24.0f, 1.3f, 24.0f };

	static constexpr float BLOCK_SIZE = 0.5f;

	static constexpr DirectX::XMINT3 BLOCK_COUNT = { 100, 100, 100 };

	static constexpr int BLOCK_KIND_COUNT = 9;

#pragma region DEBUG_MODE
	Text* mFPS = nullptr;

	Text* mModelCountText = nullptr;

	Text* mViewDirectionText = nullptr;

	Text* mVerticalVelocityText = nullptr;

	Text* mHorizontalVelocityText = nullptr;

	int mBlockCount = 0;

	Text* mBlockCountText = nullptr;

	Text* mZoom = nullptr;

	//DirectX::XMFLOAT3 mLightPosition = { 24.0f, 10.0f, 24.0f };
	DirectX::XMFLOAT3 mLightPosition = { 24.0000000f, 3.20579338f, 21.7455273f };

	bool mbMouseLock = true;
#pragma endregion

#pragma region BLOCK
	DirectX::XMFLOAT3 mMarkPosition = {};

	int mBlockKind = 0;

	DirectX::XMINT3 mBoxIndex = {};

	int mNearestSideIndex = 0;

	ModelFrame* mBlockFrame = nullptr;

	Model* mBlocks[BLOCK_COUNT.z][BLOCK_COUNT.y][BLOCK_COUNT.x] = {};
#pragma endregion

#pragma region CAMERA
	DirectX::XMFLOAT3 mMoveAxisZ = { 0.0f, 0.0f, 1.0f };

	DirectX::XMFLOAT3 mMoveAxisX = { 1.0f, 0.0f, 0.0f };

	float mMoveVelocityZ = 0.0f;

	float mMoveVelocityX = 0.0f;
#pragma endregion
};