#include <string>

#include "MainScene.h"
#include "../Core/Input/Input.h"

using namespace DirectX;

MainScene::MainScene()
{
}

MainScene::~MainScene()
{
}

void MainScene::Initialize()
{
	GetSkyDome()->SetActive(false);
	GetCamera()->SetPosition(XMFLOAT3(0.0f, 0.0f, -5.0f));

	// Create debugging texts
	{
		mFrameText = Text::Create();
		mFrameText->SetVerticalAnchor(Text::VerticalAnchor::Top);
		mFrameText->SetHorizontalAnchor(Text::HorizontalAnchor::Left);
		mFrameText->SetPosition({ 10.0f, -10.0f });
		AddText(mFrameText);

		mViewDirectionText = Text::Create();
		mViewDirectionText->SetVerticalAnchor(Text::VerticalAnchor::Top);
		mViewDirectionText->SetHorizontalAnchor(Text::HorizontalAnchor::Left);
		mViewDirectionText->SetPosition({ 10.0f, -30.0f });
		AddText(mViewDirectionText);
	}

	// Create log and
	{
		auto logTopMaterial = Material::Create("Shaders/BlockVS.hlsl", "Shaders/BlockPS.hlsl");
		logTopMaterial->RegisterTexture(0, "Resource/oak_log_top.DDS");
		logTopMaterial->RegisterBuffer(0, sizeof(XMVECTOR))
		const size_t logTopMaterialId = AddMaterial(logTopMaterial);

		auto logMaterial = Material::Create("Shaders/BlockVS.hlsl", "Shaders/BlockPS.hlsl");
		logMaterial->RegisterTexture(0, "Resource/oak_log.DDS");
		const size_t logMaterialId = AddMaterial(logMaterial);

		for (float x = 0.0f; x < 10.0f; ++x)
		{
			for (float z = 0.0f; z < 10.0f; ++z)
			{
				Model* log = Model::Create("Resource/Block.model");
				log->SetMaterial(0, logTopMaterialId);
				log->SetMaterial(1, logMaterialId);
				log->SetPosition({ x, 0.0f, z });

				AddModel(log);
			}
		}
	}
}

void MainScene::Update(const float deltaTime)
{
	UpdateCamera(deltaTime);

	// HACK: Debugging
	{
		if (Input::Get().GetKeyDown(VK_F1))
		{
			GetSkyDome()->SetActive(!GetSkyDome()->IsActive());
		}

		if (Input::Get().GetKeyDown(VK_F2))
		{
			system("cls");
		}

		if (mFrameText->IsActive())
		{
			mFrameText->SetSentence(("DELTA_TIME: " + std::to_string(deltaTime)).c_str());
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

	static bool isCameraModeUnity = true;

	// 카메라 모드를 설정한다.
	if (Input::Get().GetKeyDown(VK_SPACE))
	{
		isCameraModeUnity = !isCameraModeUnity;
	}

	const bool isCameraRotating = (isCameraModeUnity && Input::Get().GetKey(VK_SHIFT) && Input::Get().GetMouseButton(0))
		|| isCameraModeUnity == false;

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
			const float grapY = mouseMovement.x * 0.08f;
			camera->RotateY(grapY);
		}

		if (mouseMovement.y != 0)
		{
			const float grapX = mouseMovement.y * 0.08f;
			camera->RotateX(grapX);
		}
	}

	// 카메라 이동을 처리한다.
	if (isCameraModeUnity)
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

		const XMFLOAT3 viewPosition = { 4.5f, 0.0f, 4.5f };

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
