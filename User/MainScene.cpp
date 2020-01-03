#include "MainScene.h"

#include "../Core/Graphics/Mesh/Mesh.h"

using namespace DirectX;

MainScene::MainScene()
{
}

MainScene::~MainScene()
{
}

void MainScene::Initialize()
{
	Mesh* cube = Mesh::Create("Resource/Wolf.obj");
	AddMesh(cube);

	GetCamera()->SetPosition(XMFLOAT3(0.0f, 0.0f, -5.0f));
}

void MainScene::Update(const float deltaTime)
{
	static Camera* camera = GetCamera();

	// HACK: 카메라 테스트를 위해 키보드 입력 처리는 간단히 윈도우 함수로 한다.
	{
		constexpr float accX = 0.02f;
		constexpr float accY = 0.02f;

		constexpr float maxVelocityX = 0.16f;
		constexpr float maxVelocityY = 0.16f;

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

		printf("%f\n", velocityY);
		
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

		camera->MoveForward(velocityY);
		camera->MoveX(velocityX);
	}

	// 카메라 회전을 처리한다.
	{
		POINT cursorPoint = {};
		GetCursorPos(&cursorPoint);

		static POINT beforeCursorPoint = cursorPoint;

		if (cursorPoint.x - beforeCursorPoint.x != 0.0f)
		{
			const float grapY = (cursorPoint.x - beforeCursorPoint.x) * 0.1f;
			camera->RotateY(XMConvertToRadians(grapY));

			beforeCursorPoint.x = cursorPoint.x;
		}

		if (cursorPoint.y - beforeCursorPoint.y != 0.0f)
		{
			const float grapX = (cursorPoint.y - beforeCursorPoint.y) * 0.1f;
			camera->RotateX(XMConvertToRadians(grapX));

			beforeCursorPoint.y = cursorPoint.y;
		}
	}
}
