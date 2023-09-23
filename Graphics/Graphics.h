#pragma once

#include "../Utils/Debug.h"
#include <d3d11.h>


enum class GRAPHICS_MODE {WINDOWED, FULLSCREEN};


/*

EMBEDDED GRAPHICS

*/
class Graphics {

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
		int width;
		int height;
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

		m_context->OMSetRenderTargets(1, &m_rtv, nullptr);

		D3D11_VIEWPORT vp;
		vp.Width = (FLOAT)width;
		vp.Height = (FLOAT)height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		m_context->RSSetViewports(1, &vp);

	}

	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;

	~Graphics() {

		DX_RELEASE(m_rtv);
		DX_RELEASE(m_context);
		DX_RELEASE(m_swapChain);
		DX_RELEASE(m_device);
	}


	void present() 
	{
		m_swapChain->Present(1, 0);
	}

	ID3D11Device* getDevice() const { return m_device; }
	IDXGISwapChain* getSwapChain() const { return m_swapChain; }
	ID3D11DeviceContext* getImmediateContext() const { return m_context; }
	ID3D11RenderTargetView* getRenderTargetView() const { return m_rtv; }



private:

	ID3D11Device*			m_device	= nullptr; // Used to create objects on the GPU
	ID3D11DeviceContext*	m_context	= nullptr; // Issues rendering command + actual drawing

	// DXGI is a lower-level framework that gets updated less often than D3D, because it contains
	// methods that don't really change over versions
	IDXGISwapChain*			m_swapChain = nullptr; // Flips buffers
	ID3D11RenderTargetView* m_rtv = nullptr; // Framebuffer


private:

	GRAPHICS_MODE m_cdsMode = GRAPHICS_MODE::WINDOWED;



};