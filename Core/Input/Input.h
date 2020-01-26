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
		ASSERT(0 <= virtualKey && virtualKey < VIRTUAL_KEY_COUNT, "�������� �ʴ� Ű�Դϴ�.");
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

	// button - 0: ���� ��ư, 1: ������ ��ư, 2: �߰� ��ư
	inline bool GetMouseButton(const int button) const
	{
		ASSERT(0 <= button && button < MOUSE_BUTTON_CUONT, "�������� �ʴ� ��ư�Դϴ�.");
		return mbPressedMouseButton[button];
	}

	// button - 0: ���� ��ư, 1: ������ ��ư, 2: �߰� ��ư
	bool GetMouseButtonDown(const int button);

	// button - 0: ���� ��ư, 1: ������ ��ư, 2: �߰� ��ư
	bool GetMouseButtonUp(const int button);

public:
	// �� �Լ��� ���� ���ο����� ���Ǵ� �Լ��Դϴ�.
	void Initialize(CoreKey, HWND hWnd);

	// �� �Լ��� ���� ���ο����� ���Ǵ� �Լ��Դϴ�.
	void UpdateKeyState(CoreKey, const short key, const bool bPressed);

	// �� �Լ��� ���� ���ο����� ���Ǵ� �Լ��Դϴ�.
	void UpdateMousePosition(CoreKey, const DirectX::XMINT2& mousePosition);

	// �� �Լ��� ���� ���ο����� ���Ǵ� �Լ��Դϴ�.
	void UpdateMouseButtonState(CoreKey, const int button, const bool bPressed);

	// �� �Լ��� ���� ���ο����� ���Ǵ� �Լ��Դϴ�.
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

	// 0: ����, 1: ������, 2: �߰� ��ư
	static constexpr int MOUSE_BUTTON_CUONT = 3;

	bool mbPressedMouseButton[MOUSE_BUTTON_CUONT] = {};

	bool mbMouseButtonStateChanged[MOUSE_BUTTON_CUONT] = {};
};