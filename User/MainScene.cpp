#include "MainScene.h"

#include <string>

using namespace DirectX;

MainScene::MainScene()
{
}

MainScene::~MainScene()
{
}

#include <memory>
void MainScene::Initialize()
{
	GetCamera()->SetPosition(XMFLOAT3(0.0f, 0.0f, -10.0f));

	// LightShader 기준
	{
		Material* material = Material::Create("./Shaders/LightShaderVS.hlsl", "./Shaders/LightShaderPS.hlsl");
		const size_t materialID = AddMaterial(material);

		XMVECTOR lightPosition = { -5.0f, 2.0f, 3.0f };
		material->RegisterBuffer(2, sizeof(DirectX::XMVECTOR), lightPosition);
		material->RegisterTexture(0, "./Resource/seafloor.dds");

		Mesh* wolf = Mesh::Create("./Resource/wolf.obj", materialID);
		AddMesh(wolf);

		Mesh* cube = Mesh::Create("./Resource/cube.obj", materialID);
		cube->SetPosition({ 3.0f, 0.0f, 0.0f });
		AddMesh(cube);
	}

	// BasicShader 기준
	{
		Material* material = Material::Create("./Shaders/BasicShaderVS.hlsl", "./Shaders/BasicShaderPS.hlsl");
		const int materialID = AddMaterial(material);

		Mesh* wolf = Mesh::Create("./Resource/wolf.obj", materialID);
		wolf->SetPosition({ 0.0f, 0.0f, -5.0f });
		AddMesh(wolf);

		Mesh* cube = Mesh::Create("./Resource/cube.obj", materialID);
		cube->SetPosition({ 3.0f, 0.0f, -3.0f });
		AddMesh(cube);
	}
}

void MainScene::Update(const float deltaTime)
{
	static Camera* camera = GetCamera();

	// HACK: 카메라 테스트를 위해 키보드 입력 처리는 간단히 윈도우 함수로 한다.
	{
		constexpr float accX = 0.008f;
		constexpr float accY = 0.008f;

		constexpr float maxVelocityX = 0.08f;
		constexpr float maxVelocityY = 0.08f;

		static float velocityX = 0.0f;
		static float velocityY = 0.0f;

		static int forwardDirection = 0;
		static int xDirection = 0;

		if (GetAsyncKeyState('W') 
			|| GetAsyncKeyState('S'))
		{
			forwardDirection = GetAsyncKeyState('W') ? 1 : GetAsyncKeyState('S') ? -1 : 0;

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

		if (GetAsyncKeyState('A')
			|| GetAsyncKeyState('D'))
		{
			xDirection = GetAsyncKeyState('D') ? 1 : GetAsyncKeyState('A') ? -1 : 0;

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
		POINT cursorPoint = {};
		GetCursorPos(&cursorPoint);

		static POINT beforeCursorPoint = cursorPoint;

		if (cursorPoint.x - beforeCursorPoint.x != 0.0f)
		{
			const float grapY = (cursorPoint.x - beforeCursorPoint.x) * 0.08f;
			camera->RotateY(XMConvertToRadians(grapY));

			beforeCursorPoint.x = cursorPoint.x;
		}

		if (cursorPoint.y - beforeCursorPoint.y != 0.0f)
		{
			const float grapX = (cursorPoint.y - beforeCursorPoint.y) * 0.08f;
			camera->RotateX(XMConvertToRadians(grapX));

			beforeCursorPoint.y = cursorPoint.y;
		}
	}
}
