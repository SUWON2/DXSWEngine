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
	GetCamera()->SetPosition(XMFLOAT3(0.0f, 0.0f, -5.0f));

	// Create tiles
	for (float x = 0.0f; x < 10.0f; ++x)
	{
		for (float z = 0.0f; z < 10.0f; ++z)
		{
			auto tile = Model::Create("Resource/Tile.model");
			tile->SetPosition({ x, 0.0f, z });
			const int meshCount = tile->GetMeshCount();

			Material* material1 = Material::Create("Shaders/Kenney/BasicVS.hlsl", "Shaders/Kenney/BasicPS.hlsl");
			material1->RegisterBuffer(2, sizeof(XMVECTOR), XMFLOAT3(0.9764706f, 0.7137255f, 0.4509804f));
			tile->SetMaterial(0, AddMaterial(material1));

			Material* material2 = Material::Create("Shaders/Kenney/BasicVS.hlsl", "Shaders/Kenney/BasicPS.hlsl");
			material2->RegisterBuffer(2, sizeof(XMVECTOR), XMFLOAT3(0.2666667f, 0.7098039f, 0.5529412f));
			tile->SetMaterial(1, AddMaterial(material2));

			AddModel(tile);
		}
	}

	// Create a tree
	{
		auto tree = Model::Create("Resource/Tree.model");
		tree->SetPosition({ 0.0f, 0.2f, 0.0f });
		const int meshCount = tree->GetMeshCount();

		Material* material1 = Material::Create("Shaders/Kenney/BasicVS.hlsl", "Shaders/Kenney/BasicPS.hlsl");
		material1->RegisterBuffer(2, sizeof(XMVECTOR), XMFLOAT3(0.5f, 0.7098039f, 0.5529412f));
		tree->SetMaterial(0, AddMaterial(material1));

		Material* material2 = Material::Create("Shaders/Kenney/BasicVS.hlsl", "Shaders/Kenney/BasicPS.hlsl");
		material2->RegisterBuffer(2, sizeof(XMVECTOR), XMFLOAT3(0.8784314f, 0.5333334f, 0.3607843f));
		tree->SetMaterial(1, AddMaterial(material2));

		Material* material3 = Material::Create("Shaders/Kenney/BasicVS.hlsl", "Shaders/Kenney/BasicPS.hlsl");
		material3->RegisterBuffer(2, sizeof(XMVECTOR), XMFLOAT3(1.0f, 1.0f, 1.0f));
		tree->SetMaterial(2, AddMaterial(material3));

		AddModel(tree);
	}

	// Create a tree
	{
		auto tree = Model::Create("Resource/Tree.model");
		tree->SetPosition({ 2.0f, 0.2f, 2.0f });
		const int meshCount = tree->GetMeshCount();

		Material* material1 = Material::Create("Shaders/Kenney/BasicVS.hlsl", "Shaders/Kenney/BasicPS.hlsl");
		material1->RegisterBuffer(2, sizeof(XMVECTOR), XMFLOAT3(1.0f, 0.7098039f, 0.5529412f));
		tree->SetMaterial(0, AddMaterial(material1));

		Material* material2 = Material::Create("Shaders/Kenney/BasicVS.hlsl", "Shaders/Kenney/BasicPS.hlsl");
		material2->RegisterBuffer(2, sizeof(XMVECTOR), XMFLOAT3(0.8784314f, 0.5333334f, 0.3607843f));
		tree->SetMaterial(1, AddMaterial(material2));

		Material* material3 = Material::Create("Shaders/Kenney/BasicVS.hlsl", "Shaders/Kenney/BasicPS.hlsl");
		material3->RegisterBuffer(2, sizeof(XMVECTOR), XMFLOAT3(1.0f, 1.0f, 1.0f));
		tree->SetMaterial(2, AddMaterial(material3));

		AddModel(tree);
	}

	// Create a ufo
	{
		auto ufo = Model::Create("Resource/UFO.model");
		ufo->SetRotation({ 0.0f, 0.0f, -15.0f });
		ufo->SetPosition({ 2.0f, 1.0f, 5.0f });
		const int meshCount = ufo->GetMeshCount();

		Material* material1 = Material::Create("Shaders/Kenney/BasicVS.hlsl", "Shaders/Kenney/BasicPS.hlsl");
		material1->RegisterBuffer(2, sizeof(XMVECTOR), XMFLOAT3(0.7019608f, 0.854902f, 0.8941177f));
		ufo->SetMaterial(0, AddMaterial(material1));

		Material* material2 = Material::Create("Shaders/Kenney/BasicVS.hlsl", "Shaders/Kenney/BasicPS.hlsl");
		material2->RegisterBuffer(2, sizeof(XMVECTOR), XMFLOAT3(0.454902f, 0.4117647f, 0.7019608f));
		ufo->SetMaterial(1, AddMaterial(material2));

		Material* material3 = Material::Create("Shaders/Kenney/BasicVS.hlsl", "Shaders/Kenney/BasicPS.hlsl");
		material3->RegisterBuffer(2, sizeof(XMVECTOR), XMFLOAT3(0.4740168f, 0.6086806f, 0.641f));
		ufo->SetMaterial(2, AddMaterial(material3));

		Material* material4 = Material::Create("Shaders/Kenney/BasicVS.hlsl", "Shaders/Kenney/BasicPS.hlsl");
		material4->RegisterBuffer(2, sizeof(XMVECTOR), XMFLOAT3(1.0f, 1.0f, 1.0f));
		ufo->SetMaterial(3, AddMaterial(material4));

		AddModel(ufo);
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

	// 카메라 이동을 처리한다.
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
