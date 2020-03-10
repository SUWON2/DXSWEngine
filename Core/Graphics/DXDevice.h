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

	void Present();

	ID3D11Device* GetDevice() const;

	ID3D11DeviceContext* GetDeviceContext() const;

	ID3D11RenderTargetView** GetRenderTargetView();

	ID3D11DepthStencilView* GetDepthStencilView() const;

	ID3D11DepthStencilState* GetDepthDisabledStencilState() const;

	ID3D11DepthStencilState* GetDepthStencilStateEqual() const;

	ID3D11RasterizerState* GetRasterizerState() const;

	ID3D11RasterizerState* GetRasterizerStateNoCulling() const;

	// HACK: Shadow
	ID3D11ShaderResourceView** GetShadowMap();

private:
	ID3D11Device* mDevice = nullptr;

	ID3D11DeviceContext* mDeviceContext = nullptr;

	IDXGISwapChain* mSwapChain = nullptr;

	ID3D11RenderTargetView* mRenderTargetView = nullptr;

	ID3D11DepthStencilView* mDepthStencilView = nullptr;

	ID3D11DepthStencilState* mDepthDisabledStencilState = nullptr;

	ID3D11DepthStencilState* mDepthStencilStateEqual = nullptr;

	ID3D11RasterizerState* mRasterizerState = nullptr;

	ID3D11RasterizerState* mRasterizerStateNoCulling = nullptr;

	ID3D11SamplerState* mSamplerState = nullptr;

	// HACK: Shadow
	ID3D11Texture2D* mRenderTargetTextureShadow = nullptr;
	ID3D11RenderTargetView* mRenderTargetViewShadow = nullptr;
	ID3D11ShaderResourceView* mShaderResourceViewShadow = nullptr;
	ID3D11Texture2D* mDepthStencilBufferShadow = nullptr;
	ID3D11DepthStencilView* mDepthStencilViewShadow = nullptr;
	ID3D11RasterizerState* mRasterizerStateShadow = nullptr;
	ID3D11SamplerState* mSamplerStateShadow = nullptr;
};