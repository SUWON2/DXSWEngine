#include <chrono>

#include "Core.h"
#include "Graphics/DXDevice.h"
#include "Input/Input.h"
#include "Scene/Scene.h"
#include "Graphics/RendererKey.h"
#include "../Common/Define.h"
#include "../Common/Setting.h"

Core::Core(Scene* scene)
{
	ASSERT(scene != nullptr, "The scene must not be null");

	InitializeWindows();
	mDXDevice = std::make_unique<DXDevice>(mHWnd);

	Input::Get()._Initialize({}, mHWnd);

	mScene = std::unique_ptr<Scene>(scene);
	mScene->_GetRenderer({})->InitializeManager(mDXDevice->GetDevice(), mDXDevice->GetDeviceContext());
	mScene->Initialize();
	mScene->_GetRenderer({})->SortMeshAndText();

	MSG msg = {};
	static float deltaTime = 0;
	static std::chrono::time_point<std::chrono::system_clock> startTime;

	do
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			startTime = std::chrono::system_clock::now();

			mScene->Update(deltaTime);

			Input::Get()._SetPreviousFrameMousePosition({});

			mDXDevice->BeginUpdate();

			if (mScene->_GetRenderer({})->GetSkyDome()->IsActive())
			{
				mDXDevice->TurnOffCulling();
				mDXDevice->TurnOffZBuffer();

				mScene->_GetRenderer({})->DrawSkyDome();
			}

			{
				mDXDevice->TurnOnZBuffer();
				mDXDevice->TurnOnCulling();

				mScene->_GetRenderer({})->DrawMeshAndText();
			}

			mDXDevice->EndUpdate();

			deltaTime = std::chrono::duration<float>(std::chrono::system_clock::now() - startTime).count();
		}
	}
	while (msg.message != WM_QUIT);
}

Core::~Core()
{
}

void Core::InitializeWindows()
{
	HINSTANCE hInstance = GetModuleHandle(nullptr);
	const char* applicationName = "DXSWEngine";

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = HandleWindowCommand;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = nullptr;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = nullptr;
	wcex.lpszMenuName = applicationName;
	wcex.lpszClassName = applicationName;
	wcex.hIconSm = nullptr;

	if (RegisterClassEx(&wcex) == false)
	{
		ASSERT(false, "Failed to call RegisterClassEx()");
	}

	RECT windowRectangle = { 0, 0, Setting::Get().GetWidth(), Setting::Get().GetHeight() };
	AdjustWindowRect(&windowRectangle, WS_OVERLAPPEDWINDOW, FALSE);

	mHWnd = CreateWindow(applicationName, applicationName,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT
		, windowRectangle.right - windowRectangle.left, windowRectangle.bottom - windowRectangle.top
		, nullptr, nullptr, hInstance, nullptr);

	ASSERT(mHWnd != nullptr, "Failed to CreateWindow()");

	ShowWindow(mHWnd, SW_SHOW);
}

LRESULT Core::HandleWindowCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_KEYDOWN:
		case WM_KEYUP:
			Input::Get()._UpdateKeyState({}, wParam, WM_KEYUP - message);
			return 0;

		case WM_MOUSEMOVE:
			Input::Get()._UpdateMousePosition({}, { static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam)) });
			return 0;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
			Input::Get()._UpdateMouseButtonState({}, 0, WM_LBUTTONUP - message);
			return 0;

		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
			Input::Get()._UpdateMouseButtonState({}, 1, WM_RBUTTONUP - message);
			return 0;

		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
			Input::Get()._UpdateMouseButtonState({}, 2, WM_MBUTTONUP - message);
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}