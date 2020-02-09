#pragma once

#include "../CoreKey.h"
#include "../../Common/Define.h"
#include "../../Common/Setting.h"
#include "../../Common/DirectXMath.h"

class Input final
{
public:
	Input(const Input&) = delete;

	Input& operator=(const Input&) = delete;

	static Input& Get();

	bool GetKey(const short virtualKey) const;

	bool GetKeyDown(const short virtualKey);

	bool GetKeyUp(const short virtualKey);

	const DirectX::XMINT2& GetMousePosition() const;

	const DirectX::XMINT2& GetPreviousFrameMousePosition() const;

	// button - 0: 왼쪽 버튼, 1: 오른쪽 버튼, 2: 중간 버튼
	bool GetMouseButton(const int button) const;

	// button - 0: 왼쪽 버튼, 1: 오른쪽 버튼, 2: 중간 버튼
	bool GetMouseButtonDown(const int button);

	// button - 0: 왼쪽 버튼, 1: 오른쪽 버튼, 2: 중간 버튼
	bool GetMouseButtonUp(const int button);

	int GetMouseScrollWheel() const;

	bool IsCursorVisible() const;

	bool IsCirculatingMouse() const;

	void SetVisibleCursor(const bool bVisible);

	void SetCirculatingMouse(bool bCirculating);

public:
	void _Initialize(CoreKey, HWND hWnd);

	void _Renew(CoreKey);

	void _SetKey(CoreKey, const int key, const bool bPressed);

	void _SetMousePosition(CoreKey, const DirectX::XMINT2& mousePosition);

	void _SetMouseButton(CoreKey, const int button, const bool bPressed);

	void _SetMouseScrollWheel(CoreKey, const int scrollWheel);

private:
	Input() = default;

	~Input() = default;

private:
	static constexpr int VIRTUAL_KEY_COUNT = 128;

	bool mbPressedKeys[VIRTUAL_KEY_COUNT] = {};

	bool mbKeyStateChanged[VIRTUAL_KEY_COUNT] = {};

	DirectX::XMINT2 mMousePosition = {};

	DirectX::XMINT2 mPreviousFrameMousePosition = {};

	// 0: 왼쪽, 1: 오른쪽, 2: 중간 버튼
	static constexpr int MOUSE_BUTTON_CUONT = 3;

	bool mbPressedMouseButton[MOUSE_BUTTON_CUONT] = {};

	bool mbMouseButtonStateChanged[MOUSE_BUTTON_CUONT] = {};

	bool mbVisibleCursor = true;

	bool mbCirculatingMouse = {};

	int mMouseScrollWheel = {};
};