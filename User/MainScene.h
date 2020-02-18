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
	static constexpr float BLOCK_SIZE = 0.5f;

#pragma region DEBUG_MODE
	Text* mFPS = nullptr;

	Text* mViewDirectionText = nullptr;

	Text* mVerticalVelocityText = nullptr;

	Text* mHorizontalVelocityText = nullptr;

	int mBlockCount = 0;

	Text* mBlockCountText = nullptr;

	Text* mZoom = nullptr;
#pragma endregion

#pragma region BLOCK
	Model* mMark = nullptr; // �� ����Ʈ�� ����Ű�� ���� ǥ���ϴ� �뵵�Դϴ�.

	DirectX::XMINT2 mMarkPositionIndex = {};

	ID mLogTopMaterialId = {};

	ID mLogSideMaterialId = {};

	// HACK: �켱 index�� 0�̻��� ������ ó���Ѵ�.
	int mBlockHeights[50][50]; // z, x
#pragma endregion

#pragma region CAMERA
	DirectX::XMFLOAT3 mMoveAxisZ = { 0.0f, 0.0f, 1.0f };

	DirectX::XMFLOAT3 mMoveAxisX = { 1.0f, 0.0f, 0.0f };

	float mMoveVelocityZ = 0.0f;

	float mMoveVelocityX = 0.0f;
#pragma endregion
};