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
		Graphics& gfx = engine.getGraphics();
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
				
		Graphics& gfx = WindowsEngine::getInstance().getGraphics();
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


	//void renderCube(Camera& camera) {

	//	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//	context->IASetInputLayout(effect.m_vertexLayout);
	//	// Initialiser et sélectionner les « constantes » du VS
	//	const XMMATRIX viewProj = camera.getVPMatrix();
	//	const XMMATRIX matWorldViewProj = XMMatrixTranspose(matWorld * viewProj);

	//	ShadersParams sp;
	//	sp.matWorldViewProj = matWorldViewProj;
	//	sp.matWorld = matWorld;

	//	sp.vLumiere = XMVectorSet(-10.0f, 10.0f, -10.0f, 1.0f); 
	//	sp.vCamera = XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f); 
	//	sp.vAEcl = XMVectorSet(.2f, 0.2f, 0.2f, 1.0f); // WHAT ?
	//	sp.vAMat = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f); 
	//	sp.vDEcl = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f); 
	//	sp.vDMat = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	//	context->UpdateSubresource(effect.pConstantBuffer, 0, nullptr, &sp, 0, 0);

	//	ID3DX11EffectConstantBuffer* pCB = effect.m_effect->GetConstantBufferByName("param");
	//	pCB->SetConstantBuffer(effect.pConstantBuffer); // **** Rendu de l’objet
	//	//effect.m_pass->Apply(0, context);
	//	effect.apply();
	//	// -- Draw call
	//	m.draw();
	//	//context->DrawIndexed(static_cast<UINT>(ibo.getBufferSize()), 0, 0);
	//	cube.draw();
	//}
	

	virtual void renderMesh(Camera& camera, const Mesh& mesh, const Effect& effect) override {


		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetInputLayout(effect.m_vertexLayout);

		XMMATRIX viewProj = camera.getVPMatrix();
		XMMATRIX matWorldViewProj = XMMatrixTranspose(mesh.m_worldMat * viewProj);

		ShadersParams sp;


		sp.matWorldViewProj = matWorldViewProj;
		sp.matWorld = mesh.m_worldMat;

		sp.vLumiere = XMVectorSet(-10.0f, 10.0f, -10.0f, 1.0f);
		sp.vCamera = XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f);
		sp.vAEcl = XMVectorSet(.2f, 0.2f, 0.2f, 1.0f); // WHAT ?
		sp.vAMat = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		sp.vDEcl = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		sp.vDMat = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);

		context->UpdateSubresource(effect.pConstantBuffer, 0, nullptr, &sp, 0, 0);

		ID3DX11EffectConstantBuffer* pCB = effect.m_effect->GetConstantBufferByName("param");
		pCB->SetConstantBuffer(effect.pConstantBuffer); // **** Rendu de l’objet
		effect.apply();
		mesh.draw();
	}

	

	virtual Mesh loadMeshFromFile(const std::filesystem::path& path) override {
		auto res = readMeshFromObj(path);
		return Mesh(res.first, res.second);
	}


};