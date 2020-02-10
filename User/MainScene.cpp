#include <string>

#include "MainScene.h"
#include "../Core/Input/Input.h"

using namespace DirectX;

void MainScene::Initialize()
{
	GetSkyDome()->SetActive(false);
	GetCamera()->SetPosition(XMFLOAT3(2.25f, 2.0f, 2.25f));

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
	}

	// Create log
	{
		constexpr XMFLOAT2 planeScale = { 50.0f, 50.0f };

		Material* topMaterial = Material::Create("Shaders/BlockVS.hlsl", "shaders/BlockPS.hlsl");
		topMaterial->RegisterTexture(0, "Resource/oak_log_top.DDS");
		topMaterial->RegisterBuffer<Material::ShaderType::VS>(2, sizeof(XMVECTOR), planeScale);
		const ID topMaterialId = AddMaterial(topMaterial);

		Model* plane = Model::Create("Resource/Plane.model");
		plane->SetScale({ planeScale.x, 1.0f, planeScale.y });
		plane->SetMaterial(0, topMaterialId);
		AddModel(plane);
	}
}

void MainScene::Update(const float deltaTime)
{
	UpdateCamera(deltaTime);

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
				accumulatedTime -= 1.0f;
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
	}
}

void MainScene::UpdateCamera(const float deltaTime)
{
	Camera* camera = GetCamera();

	// 카메라 모드를 설정한다.
	if (Input::Get().GetKeyDown(VK_SPACE))
	{
		mIsCameraModeUnity = !mIsCameraModeUnity;
	}

	const bool isCameraRotating = (mIsCameraModeUnity && Input::Get().GetKey(VK_SHIFT) && Input::Get().GetMouseButton(0))
		|| mIsCameraModeUnity == false;

	Input::Get().SetVisibleCursor(!isCameraRotating);
	Input::Get().SetCirculatingMouse(isCameraRotating);

	// 카메라 회전을 처리한다.
	if (isCameraRotating)
	{
		const XMINT2 mouseMovement =
		{
			Input::Get().GetMousePosition().x - Input::Get().GetPreviousFrameMousePosition().x,
			Input::Get().GetMousePosition().y - Input::Get().GetPreviousFrameMousePosition().y
		};

		if (mouseMovement.x != 0)
		{
			const float grapY = mouseMovement.x * 0.05f;
			camera->RotateY(grapY);
		}

		if (mouseMovement.y != 0)
		{
			const float grapX = mouseMovement.y * 0.05f;
			camera->RotateX(grapX);
		}
	}

	// 카메라 이동을 처리한다.
	if (mIsCameraModeUnity)
	{
		static float zoomScale = 10.0f;

		if (Input::Get().GetMouseScrollWheel() > 0)
		{
			if (zoomScale > 5.0f)
			{
				--zoomScale;
			}
		}
		else if (Input::Get().GetMouseScrollWheel() < 0)
		{
			if (zoomScale < 15.0f)
			{
				++zoomScale;
			}
		}

		constexpr XMFLOAT3 viewPosition = { 2.25f, 0.0f, 2.25f };

		const XMFLOAT3 position =
		{
			viewPosition.x - camera->GetViewDirection().x * zoomScale,
			viewPosition.y - camera->GetViewDirection().y * zoomScale,
			viewPosition.z - camera->GetViewDirection().z * zoomScale
		};

		camera->SetPosition(position);
	}
	else
	{
		constexpr float accX = 0.008f;
		constexpr float accY = 0.008f;

		constexpr float maxVelocityX = 0.08f;
		constexpr float maxVelocityY = 0.08f;

		static float velocityX = 0.0f;
		static float velocityY = 0.0f;

		static int forwardDirection = 0;
		static int xDirection = 0;

		if (Input::Get().GetKey('W') || Input::Get().GetKey('S'))
		{
			forwardDirection = Input::Get().GetKey('W') - Input::Get().GetKey('S');

			if (forwardDirection > 0)
			{
				if (velocityY < maxVelocityY)
				{
					velocityY += accY;
				}
				else
				{
					velocityY = maxVelocityY;
				}
			}
			else if (forwardDirection < 0)
			{
				if (velocityY > -maxVelocityY)
				{
					velocityY -= accY;
				}
				else
				{
					velocityY = -maxVelocityY;
				}
			}
		}
		else
		{
			if (velocityY > 0.0f)
			{
				velocityY -= accY;

				if (velocityY <= 0.0f)
				{
					velocityY = 0.0f;
					forwardDirection = 0;
				}
			}
			else if (velocityY < 0.0f)
			{
				velocityY += accY;

				if (velocityY >= 0.0f)
				{
					velocityY = 0.0f;
					forwardDirection = 0;
				}
			}
		}

		if (Input::Get().GetKey('A') || Input::Get().GetKey('D'))
		{
			xDirection = Input::Get().GetKey('D') - Input::Get().GetKey('A');

			if (xDirection > 0)
			{
				if (velocityX < maxVelocityX)
				{
					velocityX += accX;
				}
				else
				{
					velocityX = maxVelocityX;
				}
			}
			else if (xDirection < 0)
			{
				if (velocityX > -maxVelocityX)
				{
					velocityX -= accX;
				}
				else
				{
					velocityX = -maxVelocityX;
				}
			}
		}
		else
		{
			if (velocityX > 0.0f)
			{
				velocityX -= accX;

				if (velocityX <= 0.0f)
				{
					velocityX = 0.0f;
					xDirection = 0;
				}
			}
			else if (velocityX < 0.0f)
			{
				velocityX += accX;

				if (velocityX >= 0.0f)
				{
					velocityX = 0.0f;
					xDirection = 0;
				}
			}
		}

		if (xDirection != 0 && forwardDirection != 0)
		{
			constexpr float OneDivSqrt2 = 0.7071f;
			camera->MoveForward(velocityY * OneDivSqrt2);
			camera->MoveX(velocityX * OneDivSqrt2);
		}
		else
		{
			camera->MoveForward(velocityY);
			camera->MoveX(velocityX);
		}
	}
}
