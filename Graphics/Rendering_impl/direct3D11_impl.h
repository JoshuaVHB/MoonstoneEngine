#pragma once

#include "../Renderer.h"
#include "../../Platform/WindowsEngine.h"

#include <d3d11.h>

struct direct3D11_impl : public Renderer::_Impl {

public:

	direct3D11_impl() 
	{
		MS::WindowsEngine& engine = MS::WindowsEngine::getInstance();
		Graphics& gfx = engine.getGraphics();
		device = gfx.getDevice();
		context = gfx.getImmediateContext();
		swapChain = gfx.getSwapChain();
		rtv = gfx.getRenderTargetView();
		/*
		*/
	}


private:

// D3D11 CONTEXT
	ID3D11Device* device = nullptr; // Used to create objects on the GPU
	ID3D11DeviceContext* context = nullptr; // Issues rendering command + actual drawing
	IDXGISwapChain* swapChain = nullptr; // Flips buffers
	ID3D11RenderTargetView* rtv = nullptr; // Framebuffer
	

	virtual void clearScreen() override final {

		const FLOAT rgba[4] = {
		0,1,1,1
		};
		context->ClearRenderTargetView(rtv, rgba);

	}


	virtual void clearScreen(float r, float g, float b, float a) override final { 

		const FLOAT rgba[4] = {
		r,g,b,a
		};
		context->ClearRenderTargetView(rtv, rgba);

	}

};