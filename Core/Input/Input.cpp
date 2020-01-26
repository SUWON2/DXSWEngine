#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#include "Input.h"
#include "../../Common/Setting.h"

using namespace DirectX;

bool Input::GetKeyDown(const short virtualKey)
{
	ASSERT(0 <= virtualKey && virtualKey < VIRTUAL_KEY_COUNT, "�������� �ʴ� Ű�Դϴ�.");
	const bool bKeyDown = (mbKeyStateChanged[virtualKey] && mbPressedKeys[virtualKey]);

	if (bKeyDown)
	{
		mbKeyStateChanged[virtualKey] = false;
	}

	return bKeyDown;
}

bool Input::GetKeyUp(const short virtualKey)
{
	ASSERT(0 <= virtualKey && virtualKey < VIRTUAL_KEY_COUNT, "�������� �ʴ� Ű�Դϴ�.");
	const bool bKeyUp = (mbKeyStateChanged[virtualKey] && !mbPressedKeys[virtualKey]);

	if (bKeyUp)
	{
		mbKeyStateChanged[virtualKey] = false;
	}

	return bKeyUp;
}

bool Input::GetMouseButtonDown(const int button)
{
	ASSERT(0 <= button && button < MOUSE_BUTTON_CUONT, "�������� �ʴ� ��ư�Դϴ�.");
	const bool bButtonDown = (mbMouseButtonStateChanged[button] && mbPressedMouseButton[button]);

	if (bButtonDown)
	{
		mbMouseButtonStateChanged[button] = false;
	}

	return bButtonDown;
}

bool Input::GetMouseButtonUp(const int button)
{
	ASSERT(0 <= button && button < MOUSE_BUTTON_CUONT, "�������� �ʴ� ��ư�Դϴ�.");
	const bool bButtonUp = (mbMouseButtonStateChanged[button] && !mbPressedMouseButton[button]);

	if (bButtonUp)
	{
		mbMouseButtonStateChanged[button] = false;
	}

	return bButtonUp;
}

void Input::Initialize(CoreKey, HWND hWnd)
{
	ASSERT(hWnd != nullptr, "The hWnd must not be null");

	// ���콺�� ȭ�鿡 ����� ���ϵ��� �����Ѵ�.
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);
	MapWindowPoints(hWnd, nullptr, reinterpret_cast<LPPOINT>(&clientRect), 2);
	ClipCursor(&clientRect);

	ShowCursor(false);
}

void Input::UpdateKeyState(CoreKey, const short key, const bool bPressed)
{
	mbKeyStateChanged[key] = (mbPressedKeys[key] != bPressed);
	mbPressedKeys[key] = bPressed;
}

void Input::UpdateMousePosition(CoreKey, const DirectX::XMINT2& mousePosition)
{
	mPreviousFrameMousePosition = mMousePosition;
	mMousePosition = mousePosition;

	const RECT clientRect =
	{
		0,
		0,
		Setting::Get().GetWidth() - 1,
		Setting::Get().GetHeight() - 1,
	};

	// x�� ���ؿ��� ���콺�� ȭ���� ��� ��� �ݴ� �������� ���콺�� �̵���ŵ�ϴ�.
	if (mMousePosition.x >= clientRect.right)
	{
		mMousePosition.x = clientRect.left + 1;
		mPreviousFrameMousePosition.x = clientRect.left + 1;

		POINT point;
		GetCursorPos(&point);
		SetCursorPos(point.x - (clientRect.right - 1), point.y);
	}
	else if (mMousePosition.x <= clientRect.left)
	{
		mMousePosition.x = clientRect.right - 1;
		mPreviousFrameMousePosition.x = clientRect.right - 1;

		POINT point;
		GetCursorPos(&point);
		SetCursorPos(point.x + (clientRect.right - 1), point.y);
	}

	// y�� ���ؿ��� ���콺�� ȭ���� ��� ��� �ݴ� �������� ���콺�� �̵���ŵ�ϴ�.
	if (mMousePosition.y >= clientRect.bottom)
	{
		mMousePosition.y = clientRect.top + 1;
		mPreviousFrameMousePosition.y = clientRect.top + 1;

		POINT point;
		GetCursorPos(&point);
		SetCursorPos(point.x, point.y - (clientRect.bottom - 1));
	}
	else if (mMousePosition.y <= clientRect.top)
	{
		mMousePosition.y = clientRect.bottom - 1;
		mPreviousFrameMousePosition.y = clientRect.bottom - 1;

		POINT point;
		GetCursorPos(&point);
		SetCursorPos(point.x, point.y + (clientRect.bottom - 1));
	}
}

void Input::UpdateMouseButtonState(CoreKey, const int button, const bool bPressed)
{
	ASSERT(0 <= button && button < MOUSE_BUTTON_CUONT, "�������� �ʴ� ��ư�Դϴ�.");
	mbMouseButtonStateChanged[button] = (mbPressedMouseButton[button] != bPressed);
	mbPressedMouseButton[button] = bPressed;
}
