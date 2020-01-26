#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <memory>

class Scene;
class DXDevice;

class Core final
{
public:
	Core(Scene* scene);

	Core(const Core&) = delete;

	Core& operator=(const Core&) = delete;

	~Core();

private:
	static LRESULT CALLBACK HandleWindowCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void InitializeWindows();

private:
	HWND mHWnd = nullptr;

	// TODO: Scene을 여러개 관리할 수 있도록 처리하자.
	std::unique_ptr<Scene> mScene = nullptr;

	std::unique_ptr<DXDevice> mDXDevice = nullptr;
};