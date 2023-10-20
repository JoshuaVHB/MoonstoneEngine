#include "d3d11_graphics.h"

#include <d3d11.h>
#include "../Utils/Debug.h"

d3d11_graphics::d3d11_graphics(HWND hWnd, GRAPHICS_MODE mode)
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
		width = rc.right - rc.left;
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
		&m_context.swapChain,
		&m_context.device,
		nullptr, // output, fills with the actual flags (useless)
		&m_context.context
	), FAIL);

	ID3D11Texture2D* pBackBuffer;
	DX_TRY_CODE(m_context.swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer), -2);
	DX_TRY_CODE(m_context.device->CreateRenderTargetView(pBackBuffer, nullptr, &m_context.rtv), -2);
	pBackBuffer->Release();

	m_depth = DepthBuffer{m_context.device, m_context.context};
	m_depth.initDepthBuffer(width, height);
	m_depth.bind();

	m_context.context->OMSetRenderTargets(1, &m_context.rtv, m_depth.getView());
	//m_context.context->OMSetRenderTargets(1, &m_context.rtv, nullptr);
	D3D11_VIEWPORT vp{};
	vp.Width = (FLOAT)(width);
	vp.Height = (FLOAT)(height);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	m_context.context->RSSetViewports(1, &vp);

	ID3D11RasterizerState* pRasterizerState;
	D3D11_RASTERIZER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.FillMode = D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_BACK;
	m_context.device->CreateRasterizerState(&desc, &pRasterizerState);
	m_context.context->RSSetState(pRasterizerState);
	DX_RELEASE(pRasterizerState);


}

d3d11_graphics::~d3d11_graphics() {

	ID3D11Debug* debugDev = nullptr;
	m_context.device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&debugDev));
	debugDev->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

	DX_RELEASE(debugDev);
	DX_RELEASE(m_context.rtv);
	DX_RELEASE(m_context.context);
	DX_RELEASE(m_context.swapChain);
	DX_RELEASE(m_context.device);
}

void d3d11_graphics::clearFramebuffer() const noexcept
{
	static constexpr FLOAT rgba[4] = { 0,0,0,1 };
	m_context.context->ClearRenderTargetView(m_context.rtv, rgba);
}

void d3d11_graphics::present() const noexcept
{
	m_context.swapChain->Present(1, 0);
}