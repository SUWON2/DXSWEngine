#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#include "Input.h"
#include "../../Common/Setting.h"

using namespace DirectX;

Input& Input::Get()
{
	static Input input;
	return input;
}

bool Input::GetKey(const short virtualKey) const
{
	ASSERT(0 <= virtualKey && virtualKey < VIRTUAL_KEY_COUNT, "�������� �ʴ� Ű�Դϴ�.");
	return mbPressedKeys[virtualKey];
}

bool Input::GetKeyDown(const short virtualKey)
{
	ASSERT(0 <= virtualKey && virtualKey < VIRTUAL_KEY_COUNT, "�������� �ʴ� Ű�Դϴ�.");
	return mbKeyStateChanged[virtualKey] && mbPressedKeys[virtualKey];
}

bool Input::GetKeyUp(const short virtualKey)
{
	ASSERT(0 <= virtualKey && virtualKey < VIRTUAL_KEY_COUNT, "�������� �ʴ� Ű�Դϴ�.");
	return mbKeyStateChanged[virtualKey] && !mbPressedKeys[virtualKey];
}

const DirectX::XMINT2& Input::GetMousePosition() const
{
	return mMousePosition;
}

const DirectX::XMINT2& Input::GetPreviousFrameMousePosition() const
{
	return mPreviousFrameMousePosition;
}

bool Input::GetMouseButton(const int button) const
{
	ASSERT(0 <= button && button < MOUSE_BUTTON_CUONT, "�������� �ʴ� ��ư�Դϴ�.");
	return mbPressedMouseButton[button];
}

bool Input::GetMouseButtonDown(const int button)
{
	ASSERT(0 <= button && button < MOUSE_BUTTON_CUONT, "�������� �ʴ� ��ư�Դϴ�.");
	return mbMouseButtonStateChanged[button] && mbPressedMouseButton[button];
}

bool Input::GetMouseButtonUp(const int button)
{
	ASSERT(0 <= button && button < MOUSE_BUTTON_CUONT, "�������� �ʴ� ��ư�Դϴ�.");
	return mbMouseButtonStateChanged[button] && !mbPressedMouseButton[button];
}

int Input::GetMouseScrollWheel() const
{
	return mMouseScrollWheel;
}

bool Input::IsCursorVisible() const
{
	return mbVisibleCursor;
}

bool Input::IsCirculatingMouse() const
{
	return mbCirculatingMouse;
}

void Input::SetVisibleCursor(const bool bVisible)
{
	if (mbVisibleCursor != bVisible)
	{
		mbVisibleCursor = bVisible;
		ShowCursor(mbVisibleCursor);
	}
}

void Input::SetCirculatingMouse(bool bCirculating)
{
	mbCirculatingMouse = bCirculating;
}

void Input::_Initialize(CoreKey, HWND hWnd)
{
	ASSERT(hWnd != nullptr, "The hWnd must not be null");

	// ���콺�� ȭ�鿡 ����� ���ϵ��� �����Ѵ�.
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);
	MapWindowPoints(hWnd, nullptr, reinterpret_cast<LPPOINT>(&clientRect), 2);
	ClipCursor(&clientRect);
}

void Input::_Renew(CoreKey)
{
	memset(mbKeyStateChanged, false, sizeof(bool) * VIRTUAL_KEY_COUNT);
	memset(mbMouseButtonStateChanged, false, sizeof(bool) * MOUSE_BUTTON_CUONT);

	mMouseScrollWheel = 0;
	mPreviousFrameMousePosition = mMousePosition;
}

void Input::_SetKey(CoreKey, const int key, const bool bPressed)
{
	mbKeyStateChanged[key] = (mbPressedKeys[key] != bPressed);
	mbPressedKeys[key] = bPressed;
}

void Input::_SetMousePosition(CoreKey, const DirectX::XMINT2& mousePosition)
{
	mPreviousFrameMousePosition = mMousePosition;
	mMousePosition = mousePosition;

	if (mbCirculatingMouse)
	{
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
}

void Input::_SetMouseButton(CoreKey, const int button, const bool bPressed)
{
	ASSERT(0 <= button && button < MOUSE_BUTTON_CUONT, "�������� �ʴ� ��ư�Դϴ�.");
	mbMouseButtonStateChanged[button] = (mbPressedMouseButton[button] != bPressed);
	mbPressedMouseButton[button] = bPressed;
}

void Input::_SetMouseScrollWheel(CoreKey, const int scrollWheel)
{
	mMouseScrollWheel += scrollWheel;
}