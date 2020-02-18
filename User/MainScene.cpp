#include <string>
#include <cmath>

#include "MainScene.h"
#include "../Core/Input/Input.h"

using namespace DirectX;

void MainScene::Initialize()
{
	GetSkyDome()->SetActive(false);
	GetCamera()->SetPosition(XMFLOAT3(0.0f, 1.3f, 0.0f));

	// Create debugging texts
	{
		mFPS = Text::Create();
		mFPS->SetVerticalAnchor(Text::VerticalAnchor::Top);
		mFPS->SetHorizontalAnchor(Text::HorizontalAnchor::Left);
		mFPS->SetPosition({ 10.0f, -10.0f });
		mFPS->SetSentence("FPS: 0");
		AddText(mFPS);

		mViewDirectionText = Text::Create();
		mViewDirectionText->SetVerticalAnchor(Text::VerticalAnchor::Top);
		mViewDirectionText->SetHorizontalAnchor(Text::HorizontalAnchor::Left);
		mViewDirectionText->SetPosition({ 10.0f, -30.0f });
		AddText(mViewDirectionText);

		mVerticalVelocityText = Text::Create();
		mVerticalVelocityText->SetVerticalAnchor(Text::VerticalAnchor::Top);
		mVerticalVelocityText->SetHorizontalAnchor(Text::HorizontalAnchor::Left);
		mVerticalVelocityText->SetPosition({ 10.0f, -50.0f });
		AddText(mVerticalVelocityText);

		mHorizontalVelocityText = Text::Create();
		mHorizontalVelocityText->SetVerticalAnchor(Text::VerticalAnchor::Top);
		mHorizontalVelocityText->SetHorizontalAnchor(Text::HorizontalAnchor::Left);
		mHorizontalVelocityText->SetPosition({ 10.0f, -70.0f });
		AddText(mHorizontalVelocityText);

		mBlockCountText = Text::Create();
		mBlockCountText->SetVerticalAnchor(Text::VerticalAnchor::Top);
		mBlockCountText->SetHorizontalAnchor(Text::HorizontalAnchor::Left);
		mBlockCountText->SetPosition({ 10.0f, -90.0f });
		AddText(mBlockCountText);

		mZoom = Text::Create();
		mZoom->SetVerticalAnchor(Text::VerticalAnchor::Middle);
		mZoom->SetHorizontalAnchor(Text::HorizontalAnchor::Center);
		mZoom->SetSentence("+");
		AddText(mZoom);
	}

	// Create a plane
	{
		constexpr XMFLOAT2 planeScale = { 50.0f, 50.0f };

		Material* material = Material::Create("Shaders/BlockVS.hlsl", "shaders/BlockPS.hlsl");
		material->RegisterTexture(0, "Resource/oak_log_top.DDS");
		material->RegisterBuffer<Material::ShaderType::VS>(2, sizeof(XMVECTOR), XMFLOAT2{ planeScale.x * 2.0f, planeScale.y * 2.0f });
		material->RegisterBuffer<Material::ShaderType::PS>(0, sizeof(XMVECTOR), XMFLOAT3({ 0.0f, 1.0f, 0.0f }));
		const ID materialId = AddMaterial(material);

		Model* plane = Model::Create("Resource/Plane.model");
		plane->SetScale({ planeScale.x, 1.0f, planeScale.y });
		plane->SetMaterial(0, materialId);
		AddModel(plane);
	}

	// Create a mark
	{
		Material* material = Material::Create("Shaders/BasicShaderVS.hlsl", "shaders/BasicShaderPS.hlsl");
		material->RegisterTexture(0, "Resource/New Piskel.DDS");
		const ID materialId = AddMaterial(material);

		mMark = Model::Create("Resource/Model.model");
		mMark->SetMaterial(0, materialId);
		mMark->SetScale({ BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE });
		AddModel(mMark);
	}

	// Create block material
	{
		Material* topMaterial = Material::Create("Shaders/BlockVS.hlsl", "shaders/BlockPS.hlsl");
		topMaterial->RegisterTexture(0, "Resource/oak_log_top.DDS");
		topMaterial->RegisterBuffer<Material::ShaderType::VS>(2, sizeof(XMVECTOR), XMFLOAT2{ 1.0f, 1.0f });
		topMaterial->RegisterBuffer<Material::ShaderType::PS>(0, sizeof(XMVECTOR), XMFLOAT3({ 1.0f, 1.0f, 1.0f }));
		mLogTopMaterialId = AddMaterial(topMaterial);

		Material* sideMaterial = Material::Create("Shaders/BlockVS.hlsl", "shaders/BlockPS.hlsl");
		sideMaterial->RegisterTexture(0, "Resource/oak_log.DDS");
		sideMaterial->RegisterBuffer<Material::ShaderType::VS>(2, sizeof(XMVECTOR), XMFLOAT2{ 1.0f, 1.0f });
		sideMaterial->RegisterBuffer<Material::ShaderType::PS>(0, sizeof(XMVECTOR), XMFLOAT3({ 1.0f, 1.0f, 1.0f }));
		mLogSideMaterialId = AddMaterial(sideMaterial);
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

	mMark->SetActive(false);

	if (Input::Get().GetMouseButton(1))
	{
		bool bCollision = false;

		// 일정한 간격으로 포인트들을 쏘아 블럭과 충돌되는 곳에 mark를 놓습니다.
		for (int i = 0; i < 85; ++i)
		{
			constexpr float pointInterval = BLOCK_SIZE * 0.1f;
			const float pointDistance = i * pointInterval;

			XMFLOAT3 collsionPoint = GetCamera()->GetPosition();
			collsionPoint.x += GetCamera()->GetViewDirection().x * pointDistance;
			collsionPoint.y += GetCamera()->GetViewDirection().y * pointDistance;
			collsionPoint.z += GetCamera()->GetViewDirection().z * pointDistance;

			for (int z = 0; z < 50; ++z)
			{
				for (int x = 0; x < 50; ++x)
				{
					const int blockHeight = mBlockHeights[z][x];

					if (blockHeight == 0)
					{
						continue;
					}

					if ((x * BLOCK_SIZE <= collsionPoint.x && collsionPoint.x < x * BLOCK_SIZE + BLOCK_SIZE)
						&& (0 <= collsionPoint.y && collsionPoint.y < blockHeight * BLOCK_SIZE)
						&& (z * BLOCK_SIZE <= collsionPoint.z && collsionPoint.z < z * BLOCK_SIZE + BLOCK_SIZE))
					{
						constexpr float markOffset = BLOCK_SIZE * 0.5f;

						bCollision = true;

						// TODO: 뭐하는 코드인지 주석달자
						if (!(blockHeight * BLOCK_SIZE - BLOCK_SIZE <= collsionPoint.y && collsionPoint.y < blockHeight * BLOCK_SIZE))
						{
							goto ESCAPE_LOOP;
						}

						mMarkPositionIndex =
						{
							static_cast<int>(std::floorf(collsionPoint.x * 2.0f)),
							static_cast<int>(std::floorf(collsionPoint.z * 2.0f))
						};

						collsionPoint.x = markOffset + BLOCK_SIZE * mMarkPositionIndex.x;
						collsionPoint.z = markOffset + BLOCK_SIZE * mMarkPositionIndex.y;
						mMark->SetPosition({ collsionPoint.x, blockHeight * BLOCK_SIZE + 0.01f, collsionPoint.z });

						mMark->SetActive(true);

						goto ESCAPE_LOOP;
					}
				}
			}

		ESCAPE_LOOP:

			if (bCollision)
			{
				break;
			}
		}

		// 일정한 간격으로 포인트들을 쏘아 지면과 충돌되는 곳에 mark를 놓습니다.
		if (bCollision == false)
		{
			for (int i = 0; i < 85; ++i)
			{
				constexpr float pointInterval = BLOCK_SIZE * 0.1f;
				const float pointDistance = i * pointInterval;

				XMFLOAT3 collsionPoint = GetCamera()->GetPosition();
				collsionPoint.x += GetCamera()->GetViewDirection().x * pointDistance;
				collsionPoint.y += GetCamera()->GetViewDirection().y * pointDistance;
				collsionPoint.z += GetCamera()->GetViewDirection().z * pointDistance;

				constexpr float planeY = 0.0f;

				// 포인트가 지면과 충돌한 경우 충돌한 위치에 mark 위치를 설정합니다.
				if (planeY - BLOCK_SIZE <= collsionPoint.y && collsionPoint.y < planeY)
				{
					mMarkPositionIndex =
					{
						static_cast<int>(std::floorf(collsionPoint.x * 2.0f)),
						static_cast<int>(std::floorf(collsionPoint.z * 2.0f))
					};

					// 이미 블럭이 있는 경우 무시합니다.
					if (mBlockHeights[mMarkPositionIndex.y][mMarkPositionIndex.x] > 0)
					{
						break;
					}

					constexpr float markOffset = BLOCK_SIZE * 0.5f;
					collsionPoint.x = markOffset + BLOCK_SIZE * mMarkPositionIndex.x;
					collsionPoint.z = markOffset + BLOCK_SIZE * mMarkPositionIndex.y;
					mMark->SetPosition({ collsionPoint.x, 0.01f, collsionPoint.z });

					mMark->SetActive(true);

					break;
				}
			}
		}

		if (Input::Get().GetMouseButtonDown(0))
		{
			if (mMark->IsActive()) // 마크가 위치한 곳에 새로운 블럭을 생성합니다.
			{
				Model* block = Model::Create("Resource/Block.model");
				block->SetMaterial(0, mLogTopMaterialId);
				block->SetMaterial(1, mLogSideMaterialId);
				block->SetScale({ BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE });
				block->SetPosition({ mMark->GetPosition().x, mMark->GetPosition().y, mMark->GetPosition().z });
				AddModel(block);

				++mBlockCount;
				++mBlockHeights[mMarkPositionIndex.y][mMarkPositionIndex.x];
			}
			else
			{

			}
		}
	}

	// HACK: 디버깅 용도
	{
		if (Input::Get().GetKeyDown(VK_F1))
		{
			GetSkyDome()->SetActive(!GetSkyDome()->IsActive());
		}

		if (Input::Get().GetKeyDown(VK_F2))
		{
			mFPS->SetActive(!mFPS->IsActive());
			mViewDirectionText->SetActive(mFPS->IsActive());
			mVerticalVelocityText->SetActive(mFPS->IsActive());
			mHorizontalVelocityText->SetActive(mFPS->IsActive());
			mBlockCountText->SetActive(mFPS->IsActive());
		}

		if (Input::Get().GetKeyDown(VK_F3))
		{
			system("cls");
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
			const std::string text = ("VERTICAL_VELOCITY: " + std::to_string(mMoveVelocityZ)).c_str();
			mVerticalVelocityText->SetSentence(text.c_str());
		}

		if (mHorizontalVelocityText->IsActive())
		{
			const std::string text = ("HORIZONTAL_VELOCITY: " + std::to_string(mMoveVelocityX)).c_str();
			mHorizontalVelocityText->SetSentence(text.c_str());
		}

		if (mBlockCountText->IsActive())
		{
			mBlockCountText->SetSentence(("BLOCK_COUNT:" + std::to_string(mBlockCount)).c_str());
		}
	}
}

void MainScene::UpdateCamera(const float deltaTime)
{
	Camera* camera = GetCamera();

	// 카메라 회전을 처리합니다.
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
				mMoveVelocityZ = std::fminf(mMoveVelocityZ + acceleration, maxVelocity);
			}
			else
			{
				mMoveVelocityZ = std::fmaxf(mMoveVelocityZ - acceleration, -maxVelocity);
			}
		}
		else
		{
			// 위쪽 방향 혹은 아래쪽 방향으로 감속을 합니다.
			if (mMoveVelocityZ > 0)
			{
				mMoveVelocityZ = std::fmaxf(mMoveVelocityZ - deceleration, 0.0f);
			}
			else
			{
				mMoveVelocityZ = std::fminf(mMoveVelocityZ + deceleration, 0.0f);
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
			const float vectorMagnitude = std::sqrtf(mMoveVelocityZ * mMoveVelocityZ + mMoveVelocityX * mMoveVelocityX);

			finalMoveVelocityZ *= std::fabsf(mMoveVelocityZ) / vectorMagnitude;
			finalMoveVelocityX *= std::fabsf(mMoveVelocityX) / vectorMagnitude;
		}

		XMFLOAT3 cameraPosition = GetCamera()->GetPosition();
		cameraPosition.x += mMoveAxisZ.x * finalMoveVelocityZ;
		cameraPosition.z += mMoveAxisZ.z * finalMoveVelocityZ;
		cameraPosition.x += mMoveAxisX.x * finalMoveVelocityX;
		cameraPosition.z += mMoveAxisX.z * finalMoveVelocityX;

		GetCamera()->SetPosition(cameraPosition);
	}
}
