#pragma once

#include "../Utils/Debug.h"
#include <d3d11.h>


enum class GRAPHICS_MODE {WINDOWED, FULLSCREEN};

class Graphics {
public:

	struct RenderingContext 
	{
		ID3D11Device* device = nullptr; 
		ID3D11DeviceContext* context = nullptr; 
		IDXGISwapChain* swapChain = nullptr; // Flips buffers
		ID3D11RenderTargetView* rtv = nullptr; // Framebuffer
	};

	RenderingContext getContext() const 
	{ 
		return { m_device, m_context, m_swapChain, m_rtv };
	}

public:
	Graphics(HWND hWnd, GRAPHICS_MODE mode=GRAPHICS_MODE::WINDOWED)
	{
		m_cdsMode = mode;

		UINT deviceFlags{};
		D3D_FEATURE_LEVEL featureLevels[] = {
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_11_1
		};

#if _DEBUG
		deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif;
		int width = 0;
		int height = 0;
;		// Setup the descriptor
		DXGI_SWAP_CHAIN_DESC sd{};
		ZeroMemory(&sd, sizeof(sd));
		switch (m_cdsMode)
		{
			case GRAPHICS_MODE::WINDOWED:
					RECT rc;
					GetClientRect(hWnd, &rc);
					width= rc.right - rc.left;
					height = rc.bottom - rc.top;
					sd.BufferCount = 1; // 1 front+ 1 back
					sd.BufferDesc.Width = width;
					sd.BufferDesc.Height = height;
					sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
					sd.BufferDesc.RefreshRate.Numerator = 60;
					sd.BufferDesc.RefreshRate.Denominator = 1;
					sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // render output
					sd.OutputWindow = hWnd;
					sd.SampleDesc.Count = 1; // AA ...
					sd.SampleDesc.Quality = 0;
					sd.Windowed = TRUE;
					break;
		}
		m_width = width;
		m_height = height;


		DX_TRY_CODE(D3D11CreateDeviceAndSwapChain
		(
			nullptr, // adapter
			D3D_DRIVER_TYPE_HARDWARE, // driver type
			nullptr, // software
			deviceFlags,
			featureLevels,
			ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION,
			&sd, // descriptor
			&m_swapChain,
			&m_device,
			nullptr, // output, fills with the actual flags (useless)
			&m_context
		), FAIL);

		ID3D11Texture2D* pBackBuffer;
		DX_TRY_CODE(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer), -2);

		DX_TRY_CODE(m_device->CreateRenderTargetView(pBackBuffer, nullptr, &m_rtv), -2);
		pBackBuffer->Release();

		initDepthBuffer();

		m_context->OMSetDepthStencilState(pDSState, 1);

		D3D11_DEPTH_STENCIL_VIEW_DESC descDSView;
		ZeroMemory(&descDSView, sizeof(descDSView));
		descDSView.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDSView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSView.Texture2D.MipSlice = 0;

		auto hr = m_device->CreateDepthStencilView(m_depthTexture, &descDSView, &m_depthStencil);

		m_context->OMSetRenderTargets(1, &m_rtv, m_depthStencil);

		D3D11_VIEWPORT vp{};
		vp.Width = (FLOAT)(width);
		vp.Height = (FLOAT)(height);
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		m_context->RSSetViewports(1, &vp);
	



	}

	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;

	~Graphics() {

		DX_RELEASE(m_depthStencil);
		DX_RELEASE(m_depthTexture);
		DX_RELEASE(m_rtv);
		DX_RELEASE(m_context);
		DX_RELEASE(m_swapChain);
		DX_RELEASE(m_device);
	}

	void clearDepth() {
		if (m_depthStencil)
			m_context->ClearDepthStencilView(m_depthStencil, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	void clearFramebuffer() {


		static constexpr FLOAT rgba[4] = {0,1,1,1};
		m_context->ClearRenderTargetView(m_rtv, rgba);
	}

	void present() 
	{
		m_swapChain->Present(1, 0);
	}

	ID3D11Device* getDevice()						const noexcept { return m_device; }
	IDXGISwapChain* getSwapChain()					const noexcept { return m_swapChain; }
	ID3D11DeviceContext* getImmediateContext()		const noexcept { return m_context; }
	ID3D11RenderTargetView* getRenderTargetView()	const noexcept { return m_rtv; }
	ID3D11DepthStencilView* getDepthBuffer()		const noexcept { return m_depthStencil; }
	std::pair<int, int> getWinSize()				const noexcept { return {m_width, m_height}; }



private:

	ID3D11Device*			m_device	= nullptr; // Used to create objects on the GPU
	ID3D11DeviceContext*	m_context	= nullptr; // Issues rendering command + actual drawing

	// DXGI is a lower-level framework that gets updated less often than D3D, because it contains
	// methods that don't really change over versions
	IDXGISwapChain*			m_swapChain = nullptr; // Flips buffers
	ID3D11RenderTargetView* m_rtv = nullptr; // Framebuffer
	
	ID3D11Texture2D* m_depthTexture;
	ID3D11DepthStencilView* m_depthStencil;
	ID3D11DepthStencilState* pDSState;



	int m_width{}, m_height{};

private:

	GRAPHICS_MODE m_cdsMode = GRAPHICS_MODE::WINDOWED;


	void initDepthBuffer()
	{

			D3D11_TEXTURE2D_DESC depthTextureDesc;
			ZeroMemory(&depthTextureDesc, sizeof(depthTextureDesc));
			depthTextureDesc.Width = m_width;
			depthTextureDesc.Height = m_height;
			depthTextureDesc.MipLevels = 1;
			depthTextureDesc.ArraySize = 1;

			depthTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

			depthTextureDesc.SampleDesc.Count = 1;
			depthTextureDesc.SampleDesc.Quality = 0;
			depthTextureDesc.Usage = D3D11_USAGE_DEFAULT;
			depthTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			depthTextureDesc.CPUAccessFlags = 0;
			depthTextureDesc.MiscFlags = 0;

			DX_TRY_CODE(m_device->CreateTexture2D(&depthTextureDesc,	NULL, &m_depthTexture),	115);
			// Création de la vue du tampon de profondeur (ou de stencil)

			D3D11_DEPTH_STENCIL_DESC dsDesc;

			// Depth test parameters
			dsDesc.DepthEnable = true;
			dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

			// Stencil test parameters
			dsDesc.StencilEnable = true;
			dsDesc.StencilReadMask = 0xFF;
			dsDesc.StencilWriteMask = 0xFF;

			// Stencil operations if pixel is front-facing
			dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
			dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			// Stencil operations if pixel is back-facing
			dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
			dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			// Create depth stencil state

			auto hr = m_device->CreateDepthStencilState(&dsDesc, &pDSState);


	}

};