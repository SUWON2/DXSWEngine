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

	void BeginUpdate();

	void EndUpdate();

	void TurnOnZBuffer();

	void TurnOffZBuffer();

	void TurnOnCulling();

	void TurnOffCulling();

	ID3D11Device* GetDevice() const;

	ID3D11DeviceContext* GetDeviceContext() const;

private:
	ID3D11Device* mDevice = nullptr;

	ID3D11DeviceContext* mDeviceContext = nullptr;

	IDXGISwapChain* mSwapChain = nullptr;

	ID3D11RenderTargetView* mRenderTargetView = nullptr;

	ID3D11DepthStencilView* mDepthStencilView = nullptr;

	ID3D11DepthStencilState* mDepthStencilState = nullptr;

	ID3D11DepthStencilState* mDepthDisabledStencilState = nullptr;

	ID3D11RasterizerState* mRasterizerState = nullptr;

	ID3D11RasterizerState* mRasterizerStateNoCulling = nullptr;
};