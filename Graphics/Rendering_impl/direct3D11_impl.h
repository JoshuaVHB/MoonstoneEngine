#pragma once

#include "../../vendor/stdafx.h"
#include "../Renderer.h"
#include "../../Platform/WindowsEngine.h"

#include "../../Debug.h"
#include "../abstraction/Vertex.h"
#include "../abstraction/VertexBuffer.h"
#include "../abstraction/IndexBuffer.h"
#include "../abstraction/Shaders.h"

#include "../../Platform/IO/FileReader.h"

#include <vector>

#define D3D11_IMPL 1



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

	

	virtual void clearScreen() override final {

		
		Graphics& gfx = WindowsEngine::getInstance().getGraphics();
		gfx.clearDepth();
		gfx.clearFramebuffer();

	}


	virtual void clearScreen(float r, float g, float b, float a) override final { 

		const FLOAT rgba[4] = {
		r,g,b,a
		};
		context->ClearRenderTargetView(rtv, rgba);

	}

	//virtual void renderMesh(const Mesh& mesh) override{	}

	IndexBuffer ibo;
	VertexBuffer vbo;
	D3D11_BUFFER_DESC bd;
	ID3D11Buffer* pConstantBuffer;
	Effect effect;

	XMMATRIX matWorld = XMMatrixIdentity();

	Mesh m;
	Shader vs, ps;

	void initCube() {

		auto mesh = readMeshFromObj("res/mesh/mesh.obj");
		if (mesh.first.size() == 0) throw;
		m = Mesh(device, context, mesh.first, mesh.second);

		std::vector<Vertex> vertices;
		std::vector<uint16_t> indices;

		vbo = VertexBuffer(device, context, mesh.first);
		ibo = IndexBuffer(device, context, mesh.second);
		
		effect = Effect(device, context);
		effect.loadEffectFromFile("res/effects/MiniPhong.fx");
		matWorld *= XMMatrixRotationX(XM_PI/2.F);
		

		// Création d'un tampon pour les constantes du VS
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(matWorld);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		device->CreateBuffer(&bd, nullptr, &pConstantBuffer);


	}



	void renderCube(Camera& camera) {

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m.draw();
		context->IASetInputLayout(effect.m_vertexLayout);
		// Initialiser et sélectionner les « constantes » du VS
		const XMMATRIX viewProj = camera.getVPMatrix();
		const XMMATRIX matWorldViewProj = XMMatrixTranspose(matWorld * viewProj);

		ShadersParams sp;
		sp.matWorldViewProj = matWorldViewProj;
		sp.matWorld = matWorld;

		sp.vLumiere = XMVectorSet(-10.0f, 10.0f, -10.0f, 1.0f); 
		sp.vCamera = XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f); 
		sp.vAEcl = XMVectorSet(.2f, 0.2f, 0.2f, 1.0f); // WHAT ?
		sp.vAMat = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f); 
		sp.vDEcl = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f); 
		sp.vDMat = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);
		context->UpdateSubresource(effect.pConstantBuffer, 0, nullptr, &sp, 0, 0);

		ID3DX11EffectConstantBuffer* pCB = effect.m_effect->GetConstantBufferByName("param");
		pCB->SetConstantBuffer(effect.pConstantBuffer); // **** Rendu de l’objet
		effect.m_pass->Apply(0, context);

		// -- Draw call
		context->DrawIndexed(static_cast<UINT>(ibo.getBufferSize()), 0, 0);
	}
	

	virtual void renderMesh(Camera& camera, float deltaTime = 0.f) override {
		static bool b = false;
		if (!b)
		{
			initCube(); b = true;
		}
		//updateCube(deltaTime);
		renderCube(camera);
	
	}

	float rotation = 0.F;
	void updateCube(float deltaTime = 0) {

		rotation = rotation + ((XM_PI * 2.0f) / 3.0f * deltaTime);
		matWorld = XMMatrixRotationX(rotation);
		matWorld *= XMMatrixRotationY(rotation);
	}


};