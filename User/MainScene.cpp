#include "MainScene.h"
#include "../Core/Input/Input.h"

#include <string>

using namespace DirectX;

MainScene::MainScene()
{
}

MainScene::~MainScene()
{
}

void MainScene::Initialize()
{
	GetCamera()->SetPosition(XMFLOAT3(0.0f, 0.0f, -5.0f));

	Material* planeMaterial = Material::Create("./Shaders/BasicShaderVS.hlsl", "./Shaders/BasicShaderPS.hlsl");
	planeMaterial->RegisterTexture(0, "./Resource/block.dds");
	planeMaterial->RegisterBuffer(2, sizeof(XMVECTOR), nullptr);
	const int materialID = AddMaterial(planeMaterial);

	for (float z = 0.0f; z <= 20.0f; ++z)
	{
		for (float x = -9.5; x <= 9.5f; ++x)
		{
			auto block = Mesh::Create("Resource/Block.mesh", materialID);
			block->SetPosition({ x, -5.0f, z });
			AddMesh(block);
		}
	}

	GetSkyDome()->SetActive(false);

	// HACK: Debugging
	{
		mFrameText = Text::Create();
		mFrameText->SetVerticalAnchor(Text::VerticalAnchor::Top);
		mFrameText->SetHorizontalAnchor(Text::HorizontalAnchor::Left);
		mFrameText->SetPosition({ 10.0f, -10.0f });
		AddText(mFrameText);
	}
}

void MainScene::Update(const float deltaTime)
{
	UpdateCamera(deltaTime);

	// HACK: Debugging
	{
		if (Input::Get().GetKeyDown(VK_F1))
		{
			mFrameText->SetActive(!mFrameText->IsActive());
		}

		if (Input::Get().GetKeyDown(VK_F2))
		{
			GetSkyDome()->SetActive(!GetSkyDome()->IsActive());
		}

		if (Input::Get().GetKeyDown(VK_F3))
		{
			system("cls");
		}

		if (mFrameText->IsActive())
		{
			mFrameText->SetSentence(("DELTA_TIME: " + std::to_string(deltaTime)).c_str());
		}
	}
}

void MainScene::UpdateCamera(const float deltaTime)
{
	static Camera* camera = GetCamera();

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

	// 카메라 회전을 처리한다.
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
}
