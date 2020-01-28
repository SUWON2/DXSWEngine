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

	// Initialize depth stencil view and render target view
	{
		D3D11_TEXTURE2D_DESC depthBufferDesc = {};
		depthBufferDesc.Width = Setting::Get().GetWidth();
		depthBufferDesc.Height = Setting::Get().GetHeight();
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;

		// ���� ���ٽ� ���۸� �����մϴ�.
		ID3D11Texture2D* depthStencilBuffer = nullptr;
		HR(mDevice->CreateTexture2D(&depthBufferDesc, nullptr, &depthStencilBuffer));

		// ���� ���ٽ� �並 �����մϴ�.
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		HR(mDevice->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &mDepthStencilView));

		RELEASE_COM(depthStencilBuffer);

		ID3D11Texture2D* backBuffer = nullptr;
		HR(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));

		// ���� Ÿ�� �並 �����մϴ�.
		HR(mDevice->CreateRenderTargetView(backBuffer, nullptr, &mRenderTargetView));
		RELEASE_COM(backBuffer);

		// ������ Ÿ�� ��� ���ٽ� ���۸� ���ε��մϴ�.
		mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
	}

	// Initialize depth stencil state
	{
		CD3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;

		// �ȼ� ������ ���ٽ� ������ ���� �մϴ�.
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// �ȼ� �޸��� ���ٽ� ������ �����մϴ�.
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// ���� ���ٽ� ���¸� �����մϴ�.
		HR(mDevice->CreateDepthStencilState(&depthStencilDesc, &mDepthStencilState));
	}

	// Initialize depth disabled stencil state
	{
		CD3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = false;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;

		// �ȼ� ������ ���ٽ� ������ ���� �մϴ�.
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// �ȼ� �޸��� ���ٽ� ������ �����մϴ�.
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// ���� ���ٽ� ���¸� �����մϴ�.
		HR(mDevice->CreateDepthStencilState(&depthStencilDesc, &mDepthDisabledStencilState));
	}

	// Initialize rasterizer state
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
	}

	// Initialize culling disabled rasterizer state
	{
		D3D11_RASTERIZER_DESC rasterizerDesc;
		rasterizerDesc.AntialiasedLineEnable = false;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		rasterizerDesc.DepthBias = 0;
		rasterizerDesc.DepthBiasClamp = 0.0f;
		rasterizerDesc.DepthClipEnable = true;
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.FrontCounterClockwise = false;
		rasterizerDesc.MultisampleEnable = false;
		rasterizerDesc.ScissorEnable = false;
		rasterizerDesc.SlopeScaledDepthBias = 0.0f;
		HR(mDevice->CreateRasterizerState(&rasterizerDesc, &mRasterizerStateNoCulling));
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

	// Initialize blend
	{
		D3D11_BLEND_DESC blendDesc = {};
		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0F;

		ID3D11BlendState* blendState = nullptr;
		HR(mDevice->CreateBlendState(&blendDesc, &blendState));

		const float blendFactor[4] = {};
		mDeviceContext->OMSetBlendState(blendState, blendFactor, 0xFFFFFFFF);

		RELEASE_COM(blendState);
	}

	// ��ο��� ���� �׻� �ﰢ���� �������� �����մϴ�.
	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

DXDevice::~DXDevice()
{
	RELEASE_COM(mRasterizerStateNoCulling);
	RELEASE_COM(mRasterizerState);
	RELEASE_COM(mDepthDisabledStencilState);
	RELEASE_COM(mDepthStencilState);
	RELEASE_COM(mDepthStencilView);
	RELEASE_COM(mRenderTargetView);
	RELEASE_COM(mSwapChain);
	RELEASE_COM(mDeviceContext);
	RELEASE_COM(mDevice);
}

void DXDevice::BeginUpdate()
{
	const float color[] = { 0.84f, 0.9f, 0.96f, 1.0f };
	mDeviceContext->ClearRenderTargetView(mRenderTargetView, color);
	mDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void DXDevice::EndUpdate()
{
	HR(mSwapChain->Present(Setting::Get().IsVsync(), 0));
}

void DXDevice::TurnOnZBuffer()
{
	mDeviceContext->OMSetDepthStencilState(mDepthStencilState, 1);
}

void DXDevice::TurnOffZBuffer()
{
	mDeviceContext->OMSetDepthStencilState(mDepthDisabledStencilState, 1);
}

void DXDevice::TurnOnCulling()
{
	mDeviceContext->RSSetState(mRasterizerState);
}

void DXDevice::TurnOffCulling()
{
	mDeviceContext->RSSetState(mRasterizerStateNoCulling);
}
