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

	int m_texCount;
	int m_width, m_height;
	DepthBuffer m_depth;
	std::vector<ID3D11RenderTargetView*> m_rtv;
	std::vector<ID3D11ShaderResourceView*> m_srv;

#ifdef D3D11_IMPL
	ID3D11RenderTargetView* m_target = nullptr;
	ID3D11Texture2D* m_textures = nullptr;
	d3d11_graphics::RenderingContext m_renderContext;
#endif

public:

	Framebuffer(int texCount = 1) : m_texCount(texCount)
	{

#ifdef D3D11_IMPL
		auto& gfx = WindowsEngine::getInstance().getGraphics();
		m_renderContext = WindowsEngine::getInstance().getGraphics().getContext();

		// Get current fbo desc
		D3D11_TEXTURE2D_DESC dsc{};	
		ID3D11Texture2D* bb;		
		m_renderContext.swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&bb);
		bb->GetDesc(&dsc);
		bb->Release();

		m_width = dsc.Width;
		m_height = dsc.Height;

		dsc.ArraySize = 1;
		dsc.BindFlags |= D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;


		ID3D11Texture2D** textureArray = new ID3D11Texture2D*[m_texCount];

		for (int i = 0; i < m_texCount; ++i)
			m_renderContext.device->CreateTexture2D(&dsc, nullptr, &textureArray[i]);



		m_depth = DepthBuffer{ m_renderContext.device, m_renderContext.context };
		m_depth.initDepthBuffer(m_width, m_height);


		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc32;
		ZeroMemory(&renderTargetViewDesc32, sizeof(renderTargetViewDesc32));
		renderTargetViewDesc32.Format = dsc.Format;
		renderTargetViewDesc32.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc32.Texture2D.MipSlice = 0;

		m_rtv.resize(m_texCount);
		m_srv.resize(m_texCount);
		for (int i = 0; i < m_texCount; ++i)
			m_renderContext.device->CreateRenderTargetView(textureArray[i], &renderTargetViewDesc32, &m_rtv[i]);


		//Shader Resource View Description
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
		shaderResourceViewDesc.Format = dsc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;

		for (int i = 0; i < m_texCount; i++)
		{
			m_renderContext.device->CreateShaderResourceView(textureArray[i], &shaderResourceViewDesc, &m_srv[i]);
		}

		for (int i = 0; i < m_texCount; ++i)
			textureArray[i]->Release();
			
		delete[] textureArray;

#endif
	}

	void clear()
	{
		m_depth.clearDepth();
		for (int i = 0; i < m_texCount; i++)
		{
			const float color[4] = { 0,0,0,0 };
			m_renderContext.context->ClearRenderTargetView(m_rtv[i], color);
		}
	}

	void bind()
	{
#ifdef D3D11_IMPL
		m_renderContext.context->OMSetRenderTargets(m_texCount, &m_rtv[0], m_depth.getView());
#endif

	}

	auto getRTV() { return m_target; }
	//auto getTex() { return tex; }

	static void unbind()
	{
		auto cnt = WindowsEngine::getInstance().getGraphics().getContext();
		cnt.context->OMSetRenderTargets(1, &cnt.rtv, WindowsEngine::getInstance().getGraphics().getDepthBuffer().getView());
	}



};