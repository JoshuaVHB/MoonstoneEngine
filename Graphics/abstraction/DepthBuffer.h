#pragma once

#include <d3d11.h>
#include "../Utils/Debug.h"

class DepthBuffer {

private:
	ID3D11Texture2D* m_depthTexture = nullptr;
	ID3D11DepthStencilView* m_depthStencil = nullptr;
	ID3D11DepthStencilState* m_state = nullptr;

	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;

public:

	DepthBuffer() = default;

	DepthBuffer(ID3D11Device* _device, ID3D11DeviceContext* _context)
	{
		device = _device;
		context = _context;
	}

	~DepthBuffer()
	{
		DX_RELEASE(m_state)
			DX_RELEASE(m_depthStencil)
			DX_RELEASE(m_depthTexture)
	}

	ID3D11DepthStencilState* getState() const { return m_state; }
	ID3D11Texture2D* getTexture() const { return m_depthTexture; }
	ID3D11DepthStencilView* getView() const { return m_depthStencil; }

	void initDepthBuffer(int width, int height)
	{

		D3D11_TEXTURE2D_DESC depthTextureDesc;
		ZeroMemory(&depthTextureDesc, sizeof(depthTextureDesc));
		depthTextureDesc.Width = width;
		depthTextureDesc.Height = height;
		depthTextureDesc.MipLevels = 1;
		depthTextureDesc.ArraySize = 1;

		depthTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

		depthTextureDesc.SampleDesc.Count = 1;
		depthTextureDesc.SampleDesc.Quality = 0;
		depthTextureDesc.Usage = D3D11_USAGE_DEFAULT;
		depthTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthTextureDesc.CPUAccessFlags = 0;
		depthTextureDesc.MiscFlags = 0;

		device->CreateTexture2D(&depthTextureDesc, nullptr, &m_depthTexture);

		D3D11_DEPTH_STENCIL_DESC dsDesc;

		// Depth test parameters
		dsDesc.DepthEnable = true;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

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

		device->CreateDepthStencilState(&dsDesc, &m_state);

		D3D11_DEPTH_STENCIL_VIEW_DESC descDSView;
		ZeroMemory(&descDSView, sizeof(descDSView));
		descDSView.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDSView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSView.Texture2D.MipSlice = 0;

		device->CreateDepthStencilView(m_depthTexture, &descDSView, &m_depthStencil);

	}

	void bind() const noexcept
	{
		context->OMSetDepthStencilState(m_state, 1);
	}

	void clearDepth() const {
		if (m_depthStencil)
			context->ClearDepthStencilView(m_depthStencil, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}
};