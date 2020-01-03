#pragma once

#include <d3d11.h>

#include "../../Common/Define.h"
#include "../../Common/Setting.h"

class DXDevice final
{
public:
	DXDevice(HWND hWnd);

	DXDevice(const DXDevice&) = delete;

	DXDevice& operator=(const DXDevice&) = delete;

	~DXDevice();

	inline void BeginUpdate()
	{
		const float color[] = { 0.0f, 0.70f, 0.67f, 1.0f };
		mDeviceContext->ClearRenderTargetView(mRenderTargetView, color);
	}

	inline void EndUpdate()
	{
		HR(mSwapChain->Present(Setting::Get().IsVsync(), 0));
	}

	inline ID3D11Device* GetDevice() const
	{
		return mDevice;
	}

	inline ID3D11DeviceContext* GetDeviceContext() const
	{
		return mDeviceContext;
	}

private:
	ID3D11Device* mDevice = nullptr;

	ID3D11DeviceContext* mDeviceContext = nullptr;

	IDXGISwapChain* mSwapChain = nullptr;

	ID3D11RenderTargetView* mRenderTargetView = nullptr;

	ID3D11RasterizerState* mRasterizerState = nullptr;
};