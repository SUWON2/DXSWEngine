#include <string>
#include <vector>
#include <algorithm>
#include <noise/PerlinNoise.h>

#include "MainScene.h"
#include "../Core/Input/Input.h"

using namespace DirectX;

void MainScene::Initialize()
{
	GetSkyDome()->SetActive(false);
	GetCamera()->SetPosition(PLAYER_START_POSITION);

	// Create debugging texts
	{
		mFPS = CreateText();
		mFPS->SetVerticalAnchor(Text::VerticalAnchor::Top);
		mFPS->SetHorizontalAnchor(Text::HorizontalAnchor::Left);
		mFPS->SetPosition({ 10.0f, -10.0f });
		mFPS->SetSentence("FPS: 0");

		mModelCountText = CreateText();
		mModelCountText->SetVerticalAnchor(Text::VerticalAnchor::Top);
		mModelCountText->SetHorizontalAnchor(Text::HorizontalAnchor::Left);
		mModelCountText->SetPosition({ 10.0f, -30.0f });

		mViewDirectionText = CreateText();
		mViewDirectionText->SetVerticalAnchor(Text::VerticalAnchor::Top);
		mViewDirectionText->SetHorizontalAnchor(Text::HorizontalAnchor::Left);
		mViewDirectionText->SetPosition({ 10.0f, -50.0f });

		mVerticalVelocityText = CreateText();
		mVerticalVelocityText->SetVerticalAnchor(Text::VerticalAnchor::Top);
		mVerticalVelocityText->SetHorizontalAnchor(Text::HorizontalAnchor::Left);
		mVerticalVelocityText->SetPosition({ 10.0f, -70.0f });

		mHorizontalVelocityText = CreateText();
		mHorizontalVelocityText->SetVerticalAnchor(Text::VerticalAnchor::Top);
		mHorizontalVelocityText->SetHorizontalAnchor(Text::HorizontalAnchor::Left);
		mHorizontalVelocityText->SetPosition({ 10.0f, -90.0f });

		mBlockCountText = CreateText();
		mBlockCountText->SetVerticalAnchor(Text::VerticalAnchor::Top);
		mBlockCountText->SetHorizontalAnchor(Text::HorizontalAnchor::Left);
		mBlockCountText->SetPosition({ 10.0f, -110.0f });

		mZoom = CreateText();
		mZoom->SetVerticalAnchor(Text::VerticalAnchor::Middle);
		mZoom->SetHorizontalAnchor(Text::HorizontalAnchor::Center);
		mZoom->SetSentence("+");
	}

	// Create a plane
	{
		//constexpr XMFLOAT2 planeScale = { 50.0f, 50.0f };

		//Material* material = CreateMaterial("Shaders/PlaneVS.hlsl", "shaders/PlanePS.hlsl");
		//material->RegisterTexture(0, "Resource/Block/grass_block_top.DDS");
		//material->RegisterBuffer<Material::ShaderType::VS>(2, sizeof(XMVECTOR), XMFLOAT2{ planeScale.x * 2.0f, planeScale.y * 2.0f });
		//material->RegisterBuffer<Material::ShaderType::PS>(0, sizeof(XMVECTOR), XMFLOAT3({ 0.557f, 0.725f, 0.443f }));

		//Model* plane;
		//CreateModelFrame("Resource/Plane.model", { material })->Create(&plane);

		//plane->SetScale({ planeScale.x, 1.0f, planeScale.y });
		//plane->SetPosition({ 25.0f, 0.0f, 25.0f });
	}

	// Create blocks
	{
		Material* blockMaterial = CreateMaterial("Shaders/BlockVS.hlsl", "shaders/BlockPS.hlsl");
		blockMaterial->RegisterBuffer<Material::ShaderType::PS>(0, sizeof(XMVECTOR), XMFLOAT3({ 1.0f, 1.0f, 1.0f }));

		blockMaterial->RegisterTextureArray(0, { 16, 16 }, {
			// TOP
			"Resource/Block/stripped_birch_log_top.DDS",
			"Resource/Block/oak_planks_vertical.DDS",
			"Resource/Block/bricks.DDS",
			"Resource/Block/sand.DDS",
			"Resource/Block/jungle_log_top.DDS",
			"Resource/Block/granite_bricks.DDS",
			"Resource/Block/cobblestone1.DDS",
			"Resource/Block/hay_block_top.DDS",
			"Resource/Block/grass_block_top.DDS",

			// SIDE
			"Resource/Block/stripped_birch_log.DDS",
			"Resource/Block/oak_planks.DDS",
			"Resource/Block/bricks.DDS",
			"Resource/Block/sand.DDS",
			"Resource/Block/jungle_planks.DDS",
			"Resource/Block/granite_bricks.DDS",
			"Resource/Block/cobblestone1.DDS",
			"Resource/Block/hay_block_side.DDS",
			"Resource/Block/grass_block_top.DDS",
			});

		mBlockFrame = CreateModelFrame("Resource/Block.model", { blockMaterial, blockMaterial });

		std::vector<Model*> blocks;
		mBlockFrame->Create(2500, &blocks);

		siv::PerlinNoise perlinNoise(123);

		for (int z = 0; z < 50; ++z)
		{
			for (int x = 0; x < 50; ++x)
			{
				const int y = std::clamp(static_cast<int>(perlinNoise.accumulatedOctaveNoise2D(x / 80.0, z / 80.0, 4) * 15.0), 0, BLOCK_COUNT.y);

				const int index = x + (z * 50);

				blocks[index]->SetPosition({
					BLOCK_SIZE * 0.5f + BLOCK_SIZE * x,
					BLOCK_SIZE * y,
					BLOCK_SIZE * 0.5f + BLOCK_SIZE * z });

				blocks[index]->SetTextureIndex(0, 0);
				blocks[index]->SetTextureIndex(1, 9);

				blocks[index]->SetScale({ BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE });
			}
		}
	}

	// Set mouse state
	{
		Input::Get().SetVisibleCursor(false);
		Input::Get().SetCirculatingMouse(true);
	}
}

void MainScene::Update(const float deltaTime)
{
	UpdateCamera(deltaTime);

	// 큐브 종류를 선택합니다.
	for (int i = 0; i < BLOCK_KIND_COUNT; ++i)
	{
		if (Input::Get().GetKeyDown(i + '1'))
		{
			mBlockKind = i;

			break;
		}
	}

	const int buttonState = Input::Get().GetMouseButtonDown(0) - Input::Get().GetMouseButtonDown(1);

	if (buttonState != 0 && IsBoxCollidedByRaycast())
	{
		if (buttonState > 0)
		{
			XMINT3 markIndex = mBoxIndex;

			switch (mNearestSideIndex)
			{
				case 0:
					--markIndex.x;
					break;

				case 1:
					++markIndex.x;
					break;

				case 2:
					--markIndex.z;
					break;

				case 3:
					++markIndex.z;
					break;

				case 4:
					--markIndex.y;
					break;

				case 5:
					++markIndex.y;
					break;
			}

			if (0 <= markIndex.x && markIndex.x < BLOCK_COUNT.x
				&& 0 <= markIndex.y && markIndex.y < BLOCK_COUNT.y
				&& 0 <= markIndex.z && markIndex.z < BLOCK_COUNT.z
				&& mBlocks[markIndex.z][markIndex.y][markIndex.x] == nullptr)
			{
				// 마크가 위치한 곳에 새로운 블럭을 생성합니다.
				Model* block = nullptr;
				mBlockFrame->Create(&block);

				block->SetPosition({
					BLOCK_SIZE * 0.5f + BLOCK_SIZE * markIndex.x,
					BLOCK_SIZE * markIndex.y,
					BLOCK_SIZE * 0.5f + BLOCK_SIZE * markIndex.z });

				block->SetScale({ BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE });

				mBlocks[markIndex.z][markIndex.y][markIndex.x] = block;

				++mBlockCount;
			}
		}
		else
		{
			if (0 <= mBoxIndex.y && mBoxIndex.y < BLOCK_COUNT.y
				&& mBlocks[mBoxIndex.z][mBoxIndex.y][mBoxIndex.x] != nullptr)
			{
				// TODO: 이렇게 처리하면 메모리 누수 발생함, 오브젝트 풀링 기법을 사용하자
				mBlocks[mBoxIndex.z][mBoxIndex.y][mBoxIndex.x]->SetActive(false);
				mBlocks[mBoxIndex.z][mBoxIndex.y][mBoxIndex.x] = nullptr;

				--mBlockCount;
			}
		}
	}

	// HACK: 디버깅 용도
	{
		if (Input::Get().GetKey(VK_UP))
		{
			mLightPosition.z += 0.5f;
		}

		if (Input::Get().GetKey(VK_DOWN))
		{
			mLightPosition.z -= 0.5f;
		}
		if (Input::Get().GetKey(VK_LEFT))
		{
			mLightPosition.x -= 0.5f;
		}

		if (Input::Get().GetKey(VK_RIGHT))
		{
			mLightPosition.x += 0.5f;
		}

		if (Input::Get().GetKeyDown('J'))
		{
			GetSkyDome()->SetActive(!GetSkyDome()->IsActive());
		}

		if (Input::Get().GetKeyDown('K'))
		{
			mFPS->SetActive(!mFPS->IsActive());
			mModelCountText->SetActive(mFPS->IsActive());
			mViewDirectionText->SetActive(mFPS->IsActive());
			mVerticalVelocityText->SetActive(mFPS->IsActive());
			mHorizontalVelocityText->SetActive(mFPS->IsActive());
			mBlockCountText->SetActive(mFPS->IsActive());
		}

		if (Input::Get().GetKeyDown('L'))
		{
			system("cls");
		}

		if (Input::Get().GetKeyDown(VK_OEM_1))
		{
			mbMouseLock = !mbMouseLock;
		}

		if (mFPS->IsActive())
		{
			static int frameCount = 0;
			static float accumulatedTime = 0.0f;

			++frameCount;
			accumulatedTime += deltaTime;

			if (accumulatedTime >= 1.0f)
			{
				mFPS->SetSentence(("FPS: " + std::to_string(frameCount)).c_str());

				frameCount = 0;
				--accumulatedTime;
			}
		}

		if (mModelCountText->IsActive())
		{
			const std::string text = "MODEL_COUNT: " + std::to_string(1);
			mModelCountText->SetSentence(text.c_str());
		}

		if (mViewDirectionText->IsActive())
		{
			const XMFLOAT3 viewDirection = GetCamera()->GetViewDirection();

			const std::string text = ("VIEW_DIRECTION: ("
				+ std::to_string(viewDirection.x) + ", "
				+ std::to_string(viewDirection.y) + ", "
				+ std::to_string(viewDirection.z) + ")");

			mViewDirectionText->SetSentence(text.c_str());
		}

		if (mVerticalVelocityText->IsActive())
		{
			const std::string text = "VERTICAL_VELOCITY: " + std::to_string(mMoveVelocityZ);
			mVerticalVelocityText->SetSentence(text.c_str());
		}

		if (mHorizontalVelocityText->IsActive())
		{
			const std::string text = "HORIZONTAL_VELOCITY: " + std::to_string(mMoveVelocityX);
			mHorizontalVelocityText->SetSentence(text.c_str());
		}

		if (mBlockCountText->IsActive())
		{
			const std::string text = "BLOCK_COUNT:" + std::to_string(mBlockCount);
			mBlockCountText->SetSentence(text.c_str());
		}
	}
}

void MainScene::UpdateCamera(const float deltaTime)
{
	Camera* camera = GetCamera();

	// 카메라 회전을 처리합니다.
	if (mbMouseLock)
	{
		const XMINT2 mouseMovement =
		{
			Input::Get().GetMousePosition().x - Input::Get().GetPreviousFrameMousePosition().x,
			Input::Get().GetMousePosition().y - Input::Get().GetPreviousFrameMousePosition().y
		};

		const float grapSensitivity = 2.0f * deltaTime;

		if (mouseMovement.x != 0)
		{
			const float grapY = mouseMovement.x * grapSensitivity;
			camera->RotateY(grapY);

			// 이동축을 구합니다.
			const XMMATRIX matRotationY = XMMatrixRotationY(XMConvertToRadians(grapY));
			XMStoreFloat3(&mMoveAxisZ, XMVector3TransformNormal(XMLoadFloat3(&mMoveAxisZ), matRotationY));
			XMStoreFloat3(&mMoveAxisX, XMVector3Cross(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMLoadFloat3(&mMoveAxisZ)));
		}

		if (mouseMovement.y != 0)
		{
			const float grapX = mouseMovement.y * grapSensitivity;
			camera->RotateX(grapX);
		}
	}

	// 카메라 이동을 처리합니다.
	{
		constexpr float maxVelocity = 3.5f;
		constexpr float acceleration = 0.4f;
		constexpr float deceleration = 0.2f;

		const int zDirectionKeyState = Input::Get().GetKey('W') - Input::Get().GetKey('S');
		const int xDirectionKeyState = Input::Get().GetKey('D') - Input::Get().GetKey('A');

		if (zDirectionKeyState != 0)
		{
			// 위쪽 방향 혹은 아래쪽 방향으로 가속을 합니다.
			if (zDirectionKeyState > 0)
			{
				mMoveVelocityZ = std::fmin(mMoveVelocityZ + acceleration, maxVelocity);
			}
			else
			{
				mMoveVelocityZ = std::fmax(mMoveVelocityZ - acceleration, -maxVelocity);
			}
		}
		else
		{
			// 위쪽 방향 혹은 아래쪽 방향으로 감속을 합니다.
			if (mMoveVelocityZ > 0)
			{
				mMoveVelocityZ = std::fmax(mMoveVelocityZ - deceleration, 0.0f);
			}
			else
			{
				mMoveVelocityZ = std::fmin(mMoveVelocityZ + deceleration, 0.0f);
			}
		}

		if (xDirectionKeyState != 0)
		{
			// 왼쪽 방향 혹은 오른쪽 방향으로 가속을 합니다.
			if (xDirectionKeyState > 0)
			{
				mMoveVelocityX = std::fminf(mMoveVelocityX + acceleration, maxVelocity);
			}
			else
			{
				mMoveVelocityX = std::fmaxf(mMoveVelocityX - acceleration, -maxVelocity);
			}
		}
		else
		{
			// 왼쪽 방향 혹은 오른쪽 방향으로 감속을 합니다.
			if (mMoveVelocityX > 0)
			{
				mMoveVelocityX = std::fmaxf(mMoveVelocityX - deceleration, 0.0f);
			}
			else
			{
				mMoveVelocityX = std::fminf(mMoveVelocityX + deceleration, 0.0f);;
			}
		}

		float finalMoveVelocityZ = mMoveVelocityZ * deltaTime;
		float finalMoveVelocityX = mMoveVelocityX * deltaTime;

		// z축과 x축으로 동시에 이동하는 경우 속력을 조절합니다.
		if (mMoveVelocityZ != 0.0f && mMoveVelocityX != 0.0f)
		{
			const float vectorMagnitude = std::sqrt(mMoveVelocityZ * mMoveVelocityZ + mMoveVelocityX * mMoveVelocityX);

			finalMoveVelocityZ *= std::abs(mMoveVelocityZ) / vectorMagnitude;
			finalMoveVelocityX *= std::abs(mMoveVelocityX) / vectorMagnitude;
		}

		XMFLOAT3 cameraPosition = GetCamera()->GetPosition();
		cameraPosition.x += mMoveAxisZ.x * finalMoveVelocityZ;
		cameraPosition.z += mMoveAxisZ.z * finalMoveVelocityZ;
		cameraPosition.x += mMoveAxisX.x * finalMoveVelocityX;
		cameraPosition.z += mMoveAxisX.z * finalMoveVelocityX;

		if (Input::Get().GetKeyDown(VK_SPACE))
		{
			cameraPosition.y = PLAYER_START_POSITION.y;
		}
		else
		{
			cameraPosition.y += Input::Get().GetMouseScrollWheel() * 0.08f * deltaTime;
		}

		GetCamera()->SetPosition(cameraPosition);
	}
}

bool MainScene::IsBoxCollidedByRaycast()
{
	constexpr int pointDistanceMin = 25;
	constexpr int pointDistanceMax = 85;
	constexpr float pointInterval = BLOCK_SIZE * 0.1f;

	constexpr float nearesRaycastPoint = pointDistanceMin * pointInterval;
	constexpr float furthermostRaycastPoint = (pointDistanceMax - 1) * pointInterval;

	const XMFLOAT3& cameraPosition = GetCamera()->GetPosition();
	const XMFLOAT3& cameraViewDirection = GetCamera()->GetViewDirection();

	const XMFLOAT3 nearestBox
	{
		cameraPosition.x + cameraViewDirection.x * nearesRaycastPoint,
		cameraPosition.y + cameraViewDirection.y * nearesRaycastPoint,
		cameraPosition.z + cameraViewDirection.z * nearesRaycastPoint
	};

	XMINT3 nearestBoxIndex =
	{
		static_cast<int>(std::floor(nearestBox.x * 2.0f)),
		static_cast<int>(std::floor(nearestBox.y * 2.0f)),
		static_cast<int>(std::floor(nearestBox.z * 2.0f)),
	};

	const XMFLOAT3 furthermostBox
	{
		cameraPosition.x + cameraViewDirection.x * furthermostRaycastPoint,
		cameraPosition.y + cameraViewDirection.y * furthermostRaycastPoint,
		cameraPosition.z + cameraViewDirection.z * furthermostRaycastPoint
	};

	XMINT3 furthermostBoxIndex =
	{
		static_cast<int>(std::floor(furthermostBox.x * 2.0f)),
		static_cast<int>(std::floor(furthermostBox.y * 2.0f)),
		static_cast<int>(std::floor(furthermostBox.z * 2.0f)),
	};

	const auto Sort = [](int& a, int& b)
	{
		if (a > b)
		{
			const int temp = a;
			a = b;
			b = temp;
		}
	};

	Sort(nearestBoxIndex.x, furthermostBoxIndex.x);
	Sort(nearestBoxIndex.y, furthermostBoxIndex.y);
	Sort(nearestBoxIndex.z, furthermostBoxIndex.z);

	nearestBoxIndex.x = static_cast<int>(std::fmax(0, nearestBoxIndex.x));
	nearestBoxIndex.y = static_cast<int>(std::fmax(-1, nearestBoxIndex.y));
	nearestBoxIndex.z = static_cast<int>(std::fmax(0, nearestBoxIndex.z));

	furthermostBoxIndex.x = static_cast<int>(std::fmin(BLOCK_COUNT.x, furthermostBoxIndex.x));
	furthermostBoxIndex.y = static_cast<int>(std::fmin(BLOCK_COUNT.y, furthermostBoxIndex.y));
	furthermostBoxIndex.z = static_cast<int>(std::fmin(BLOCK_COUNT.z, furthermostBoxIndex.z));

	for (int i = pointDistanceMin; i < pointDistanceMax; ++i)
	{
		const float pointDistance = i * pointInterval;

		const XMFLOAT3 collisionPoint =
		{
			cameraPosition.x + cameraViewDirection.x * pointDistance,
			cameraPosition.y + cameraViewDirection.y * pointDistance,
			cameraPosition.z + cameraViewDirection.z * pointDistance,
		};

		const XMINT3 collisionPointIndex =
		{
			static_cast<int>(std::floor(collisionPoint.x * 2.0f)),
			static_cast<int>(std::floor(collisionPoint.y * 2.0f)),
			static_cast<int>(std::floor(collisionPoint.z * 2.0f))
		};

		for (int z = nearestBoxIndex.z; z <= furthermostBoxIndex.z; ++z)
		{
			for (int y = nearestBoxIndex.y; y <= furthermostBoxIndex.y; ++y)
			{
				for (int x = nearestBoxIndex.x; x <= furthermostBoxIndex.x; ++x)
				{
					if (x == collisionPointIndex.x
						&& y == collisionPointIndex.y
						&& z == collisionPointIndex.z)
					{
						// 충돌 포인트가 위치하는 곳에 블럭이 없으면 계속 충돌 포인트를 쏩니다.
						if (y >= 0 && mBlocks[z][y][x] == nullptr)
						{
							continue;
						}

						mBoxIndex = collisionPointIndex;

						if (Input::Get().GetKey(VK_SHIFT))
						{
							// 블럭의 옆면을 지정합니다.
							const float leftSide = x * BLOCK_SIZE;
							const float rightSide = leftSide + BLOCK_SIZE;
							const float backSide = z * BLOCK_SIZE;
							const float frontSide = backSide + BLOCK_SIZE;

							const float sideDistances[4] =
							{
								std::abs(collisionPoint.x - leftSide),
								std::abs(collisionPoint.x - rightSide),
								std::abs(collisionPoint.z - backSide),
								std::abs(collisionPoint.z - frontSide)
							};

							mNearestSideIndex = 0;

							for (int i = 1; i < 4; ++i)
							{
								if (sideDistances[mNearestSideIndex] > sideDistances[i])
								{
									mNearestSideIndex = i;
								}
							}
						}
						else
						{
							// 블럭의 윗면 혹은 밑면을 지정합니다.
							const float underSide = y * BLOCK_SIZE;
							const float upperSide = underSide + BLOCK_SIZE;

							if (std::abs(collisionPoint.y - underSide) < std::abs(collisionPoint.y - upperSide))
							{
								mNearestSideIndex = 4;
							}
							else
							{
								mNearestSideIndex = 5;
							}
						}

						return true;
					}
				}
			}
		}
	}

	return false;
}