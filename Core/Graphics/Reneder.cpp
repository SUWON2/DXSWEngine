#pragma warning(disable: 26812)

#include "Reneder.h"
#include "../../Common/Define.h"
#include "../../Common/Setting.h"

Reneder::Reneder(HWND hWnd)
{
	ASSERT(hWnd != nullptr, "The hWnd must not be null");

	InitializeDevice(hWnd);
}

Reneder::~Reneder()
{
	ReleaseCOM(mRenderTargetView);
	ReleaseCOM(mSwapChain);
	ReleaseCOM(mDeviceContext);
	ReleaseCOM(mDevice);
}

void Reneder::Draw()
{
	const float color[] = { 0.098039225f, 0.098039225f, 0.439215720f, 1.000000000f };
	mDeviceContext->ClearRenderTargetView(mRenderTargetView, color);

	HR(mSwapChain->Present(Setting::Get().IsVsync(), 0));
}

void Reneder::InitializeDevice(HWND hWnd)
{
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
		ReleaseCOM(dxgiDevice);

		IDXGIFactory1* dxgiFactory = nullptr;
		HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory)));
		ReleaseCOM(dxgiAdapter);

		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		swapChainDesc.BufferCount = 2;
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
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.Flags = 0;
		HR(dxgiFactory->CreateSwapChain(mDevice, &swapChainDesc, &mSwapChain));
		ReleaseCOM(dxgiFactory);
	}

	// Create a render target view
	{
		ID3D11Texture2D* backBuffer = nullptr;
		HR(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));

		HR(mDevice->CreateRenderTargetView(backBuffer, nullptr, &mRenderTargetView));
		ReleaseCOM(backBuffer);

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
}
