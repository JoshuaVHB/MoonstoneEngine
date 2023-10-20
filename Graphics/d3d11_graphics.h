#pragma once

#include <d3d11.h>
#include <utility>
#include "abstraction/DepthBuffer.h"


enum GRAPHICS_MODE { WINDOWED, FULLSCREEN };

class d3d11_graphics {

public:

	struct RenderingContext
	{
		ID3D11Device* device = nullptr;
		ID3D11DeviceContext* context = nullptr;
		IDXGISwapChain* swapChain = nullptr;
		ID3D11RenderTargetView* rtv = nullptr;
	};

	RenderingContext getContext() const { return m_context; }


private:

	DepthBuffer m_depth;
	RenderingContext m_context;
	int m_width{}, m_height{};
	GRAPHICS_MODE m_cdsMode = GRAPHICS_MODE::WINDOWED;

public:

	///////////////////////////////////////////////////////////////////////////
	/// -- Constructors

	d3d11_graphics(HWND hWnd, GRAPHICS_MODE mode = GRAPHICS_MODE::WINDOWED);
	d3d11_graphics(const d3d11_graphics&) = delete;
	d3d11_graphics& operator=(const d3d11_graphics&) = delete;
	~d3d11_graphics();

	///////////////////////////////////////////////////////////////////////////
	/// -- Basic operations

	void clearDepth() const noexcept { m_depth.clearDepth(); }
	void clearFramebuffer() const noexcept;
	void present()const noexcept;

	///////////////////////////////////////////////////////////////////////////
	/// -- Getters

	const DepthBuffer &getDepthBuffer()				const noexcept { return m_depth; }
	ID3D11Device* getDevice()						const noexcept { return m_context.device; }
	IDXGISwapChain* getSwapChain()					const noexcept { return m_context.swapChain; }
	ID3D11DeviceContext* getImmediateContext()		const noexcept { return m_context.context; }
	ID3D11RenderTargetView* getRenderTargetView()	const noexcept { return m_context.rtv; }
	std::pair<int, int> getWinSize()				const noexcept { return { m_width, m_height }; }

};