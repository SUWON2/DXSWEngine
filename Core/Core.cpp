#include "Core.h"
#include "../Common/Define.h"
#include "../Common/Setting.h"
#include "Graphics/Reneder.h"

static LRESULT CALLBACK HandleWindowCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

Core::Core()
{
	InitializeWindows();

	mReneder = std::make_unique<Reneder>(mHWnd);

	MSG msg = { 0 };
	do
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			mReneder->Draw();
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

LRESULT CALLBACK HandleWindowCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}
