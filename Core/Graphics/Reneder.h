#pragma once

#include <d3d11.h>
#include <dxgi.h>

class Reneder final
{
public:
	Reneder(HWND hWnd);

	Reneder(const Reneder&) = default;

	Reneder& operator=(const Reneder&) = delete;

	~Reneder();

	void Draw();

private:
	void InitializeDevice(HWND hWnd);

private:
	ID3D11Device* mDevice = nullptr;

	ID3D11DeviceContext* mDeviceContext = nullptr;

	IDXGISwapChain* mSwapChain = nullptr;

	ID3D11RenderTargetView* mRenderTargetView = nullptr;
};