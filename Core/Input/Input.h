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

	static inline Input& Get()
	{
		static Input input;
		return input;
	}

	inline bool GetKey(const short virtualKey) const
	{
		ASSERT(0 <= virtualKey && virtualKey < VIRTUAL_KEY_COUNT, "지원하지 않는 키입니다.");
		return mbPressedKeys[virtualKey];
	}

	bool GetKeyDown(const short virtualKey);

	bool GetKeyUp(const short virtualKey);

	inline const DirectX::XMINT2& GetMousePosition() const
	{
		return mMousePosition;
	}

	inline const DirectX::XMINT2& GetPreviousFrameMousePosition() const
	{
		return mPreviousFrameMousePosition;
	}

	// button - 0: 왼쪽 버튼, 1: 오른쪽 버튼, 2: 중간 버튼
	inline bool GetMouseButton(const int button) const
	{
		ASSERT(0 <= button && button < MOUSE_BUTTON_CUONT, "지원하지 않는 버튼입니다.");
		return mbPressedMouseButton[button];
	}

	// button - 0: 왼쪽 버튼, 1: 오른쪽 버튼, 2: 중간 버튼
	bool GetMouseButtonDown(const int button);

	// button - 0: 왼쪽 버튼, 1: 오른쪽 버튼, 2: 중간 버튼
	bool GetMouseButtonUp(const int button);

public:
	// 이 함수는 엔진 내부에서만 사용되는 함수입니다.
	void Initialize(CoreKey, HWND hWnd);

	// 이 함수는 엔진 내부에서만 사용되는 함수입니다.
	void UpdateKeyState(CoreKey, const short key, const bool bPressed);

	// 이 함수는 엔진 내부에서만 사용되는 함수입니다.
	void UpdateMousePosition(CoreKey, const DirectX::XMINT2& mousePosition);

	// 이 함수는 엔진 내부에서만 사용되는 함수입니다.
	void UpdateMouseButtonState(CoreKey, const int button, const bool bPressed);

	// 이 함수는 엔진 내부에서만 사용되는 함수입니다.
	inline void SetPreviousFrameMousePosition(CoreKey)
	{
		mPreviousFrameMousePosition = mMousePosition;
	}

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
};