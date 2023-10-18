#pragma once

#include <filesystem>
#include <utility>
#include <d3d11.h>

#include "../../Utils/Debug.h"
#include "../../Platform/WindowsEngine.h"
#include "Texture.h"


class Framebuffer 

{

private:

	int m_width, m_height;

#ifdef D3D11_IMPL
	ID3D11RenderTargetView* m_target = nullptr;
	ID3D11Texture2D* m_result = nullptr;


	ID3D11Texture2D* m_depth = nullptr;
	Texture tex;
	ID3D11DepthStencilView* m_depthStencil;
	ID3D11DepthStencilState* pDSState;
	ID3D11Texture2D* pBackBuffer = nullptr;
	d3d11_graphics::RenderingContext m_renderContext;
#endif

public:

	Framebuffer()
	{

#ifdef D3D11_IMPL
		auto& gfx = WindowsEngine::getInstance().getGraphics();
		m_renderContext = WindowsEngine::getInstance().getGraphics().getContext();

		D3D11_TEXTURE2D_DESC dsc{

		};

		m_renderContext.swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		pBackBuffer->GetDesc(&dsc);

		dsc.BindFlags |= D3D11_BIND_RENDER_TARGET;

		m_renderContext.device->CreateTexture2D(&dsc, nullptr, &m_result);


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
		m_renderContext.device->CreateTexture2D(&depthTextureDesc, NULL, &m_depth);


		m_renderContext.context->OMGetDepthStencilState(&pDSState, 0);

		D3D11_DEPTH_STENCIL_VIEW_DESC descDSView;
		ZeroMemory(&descDSView, sizeof(descDSView));
		descDSView.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDSView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSView.Texture2D.MipSlice = 0;

		auto hr = m_renderContext.device->CreateDepthStencilView(m_depth, &descDSView, &m_depthStencil);



		m_renderContext.device->CreateRenderTargetView(m_result, nullptr, &m_target);
#endif
	}

	void bind()
	{
#ifdef D3D11_IMPL
		m_renderContext.context->OMSetRenderTargets(1, &m_target, m_depthStencil);
#endif

	}

	auto getRTV() { return m_target; }
	auto getTex() { return tex; }

	static void unbind()
	{
		auto cnt = WindowsEngine::getInstance().getGraphics().getContext();
		cnt.context->OMSetRenderTargets(1, &cnt.rtv, WindowsEngine::getInstance().getGraphics().getDepthBuffer());
	}



};