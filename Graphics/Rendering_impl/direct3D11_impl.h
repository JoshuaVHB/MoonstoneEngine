#pragma once

#include "../../vendor/stdafx.h"
#include "../Renderer.h"
#include "../../Platform/WindowsEngine.h"

#include "../../Debug.h"
#include "../abstraction/Vertex.h"
#include "../abstraction/VertexBuffer.h"
#include "../abstraction/IndexBuffer.h"
#include "../abstraction/Shaders.h"
#include "../abstraction/Camera.h"

#include "../../Platform/IO/FileReader.h"
#include "../../Graphics/World/RessourcesManager.h"
#include "../World/Cube.h"

#include <vector>

struct direct3D11_impl : public Renderer::_Impl {

public:

	direct3D11_impl() 
	{
		WindowsEngine& engine = WindowsEngine::getInstance();
		d3d11_graphics& gfx = engine.getGraphics();
		device = gfx.getDevice();
		context = gfx.getImmediateContext();
		swapChain = gfx.getSwapChain();
		rtv = gfx.getRenderTargetView();

	}


private:

	// D3D11 CONTEXT
	ID3D11Device* device = nullptr; // Used to create objects on the GPU
	ID3D11DeviceContext* context = nullptr; // Issues rendering command + actual drawing
	IDXGISwapChain* swapChain = nullptr; // Flips buffers
	ID3D11RenderTargetView* rtv = nullptr; // Framebuffer

private:

	virtual void clearScreen() override final {
				
		d3d11_graphics& gfx = WindowsEngine::getInstance().getGraphics();
		gfx.clearDepth();
		gfx.clearFramebuffer();

	}


	virtual void clearScreen(float r, float g, float b, float a) override final { 
		const FLOAT rgba[4] = {r,g,b,a};
		context->ClearRenderTargetView(rtv, rgba);
	}

	virtual void drawIndexed(size_t count, uint32_t startIndexLocation, uint32_t baseVertexLocation) override {
		context->DrawIndexed(static_cast<UINT>(count), startIndexLocation, baseVertexLocation);
	}


	virtual void renderMesh(Camera& camera, const Mesh& mesh, const Effect& effect) override {

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetInputLayout(effect.m_vertexLayout);

		//effect.updateSubresource(mesh.m_worldMat, "meshParams"); // TODO make this more flexible
		//effect.sendCBufferToGPU("meshParams");
		effect.apply();
		mesh.draw();
	}

	

	virtual Mesh loadMeshFromFile(const std::filesystem::path& path) override {
		auto res = readMeshFromObj(path);
		return Mesh(res.first, res.second);
	}


};