#pragma warning(disable: 26812)

#include "DXDevice.h"

DXDevice::DXDevice(HWND hWnd)
{
	ASSERT(hWnd != nullptr, "The hWnd must not be null");

	// Create device and device context
	{
		UINT flags = 0;

#if defined(DEBUG) | defined(_DEBUG)
		flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		const D3D_DRIVER_TYPE driverTypes[] =
		{
			D3D_DRIVER_TYPE_HARDWARE,
			D3D_DRIVER_TYPE_WARP,
			D3D_DRIVER_TYPE_REFERENCE
		};

		const D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
		};
		constexpr UINT featureLevelCount = sizeof(featureLevels) / sizeof(D3D_FEATURE_LEVEL);

		HRESULT hCreatingDevice = S_OK;

		for (const auto driverType : driverTypes)
		{
			hCreatingDevice = D3D11CreateDevice(nullptr, driverType, nullptr, flags, featureLevels, featureLevelCount,
				D3D11_SDK_VERSION, &mDevice, nullptr, &mDeviceContext);

			if (SUCCEEDED(hCreatingDevice))
			{
				break;
			}
		}

		HR(hCreatingDevice);
	}

	// Obtain DXGI factory from device and Create swap chain
	{
		IDXGIDevice* dxgiDevice = nullptr;
		HR(mDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice)));

		IDXGIAdapter* dxgiAdapter = nullptr;
		HR(dxgiDevice->GetAdapter(&dxgiAdapter));
		RELEASE_COM(dxgiDevice);

		IDXGIFactory1* dxgiFactory = nullptr;
		HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory)));
		RELEASE_COM(dxgiAdapter);

		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Width = Setting::Get().GetWidth();
		swapChainDesc.BufferDesc.Height = Setting::Get().GetHeight();
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = hWnd;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.Windowed = Setting::Get().IsWindow();
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;
		HR(dxgiFactory->CreateSwapChain(mDevice, &swapChainDesc, &mSwapChain));
		RELEASE_COM(dxgiFactory);
	}

	// Create a render target view
	{
		ID3D11Texture2D* backBuffer = nullptr;
		HR(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));

		HR(mDevice->CreateRenderTargetView(backBuffer, nullptr, &mRenderTargetView));
		RELEASE_COM(backBuffer);

		mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, nullptr);
	}

	// Set the viewport
	{
		D3D11_VIEWPORT viewPort;
		viewPort.Width = static_cast<float>(Setting::Get().GetWidth());
		viewPort.Height = static_cast<float>(Setting::Get().GetHeight());
		viewPort.MinDepth = 0.0f;
		viewPort.MaxDepth = 1.0f;
		viewPort.TopLeftX = 0;
		viewPort.TopLeftY = 0;
		mDeviceContext->RSSetViewports(1, &viewPort);
	}

	// Initialize Rasterizer
	{
		D3D11_RASTERIZER_DESC rasterizerDesc;
		rasterizerDesc.AntialiasedLineEnable = false;
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		rasterizerDesc.DepthBias = 0;
		rasterizerDesc.DepthBiasClamp = 0.0f;
		rasterizerDesc.DepthClipEnable = true;
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.FrontCounterClockwise = false;
		rasterizerDesc.MultisampleEnable = false;
		rasterizerDesc.ScissorEnable = false;
		rasterizerDesc.SlopeScaledDepthBias = 0.0f;
		HR(mDevice->CreateRasterizerState(&rasterizerDesc, &mRasterizerState));

		mDeviceContext->RSSetState(mRasterizerState);
	}

	// Initialize sampler state
	{
		D3D11_SAMPLER_DESC samplerStateDesc = {};
		samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerStateDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerStateDesc.MinLOD = 0;
		samplerStateDesc.MaxLOD = D3D11_FLOAT32_MAX;

		ID3D11SamplerState* samplerState = nullptr;
		HR(mDevice->CreateSamplerState(&samplerStateDesc, &samplerState));

		mDeviceContext->PSSetSamplers(0, 1, &samplerState);
		RELEASE_COM(samplerState);
	}

	// 드로우할 때는 항상 삼각형을 기준으로 지정합니다.
	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

DXDevice::~DXDevice()
{
	RELEASE_COM(mRasterizerState);
	RELEASE_COM(mRenderTargetView);
	RELEASE_COM(mSwapChain);
	RELEASE_COM(mDeviceContext);
	RELEASE_COM(mDevice);
}