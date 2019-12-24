#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <memory>

class Reneder;

class Core final
{
public:
	Core();

	Core(const Core&) = delete;

	Core& operator=(const Core&) = delete;

	~Core();

private:
	void InitializeWindows();	

private:
	HWND mHWnd = nullptr;

	std::unique_ptr<Reneder> mReneder = nullptr;
};