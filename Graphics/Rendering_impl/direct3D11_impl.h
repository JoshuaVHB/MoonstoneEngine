#pragma once

#include "../../vendor/stdafx.h"
#include "../Renderer.h"
#include "../../Platform/WindowsEngine.h"

#include "../../Debug.h"
#include "../abstraction/Vertex.h"
#include "../abstraction/VertexBuffer.h"
#include "../abstraction/IndexBuffer.h"
#include "../abstraction/Shaders.h"

#include <vector>

#define D3D11_IMPL 1



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

	//virtual void renderMesh(const Mesh& mesh) override{	}

	IndexBuffer ibo;
	VertexBuffer vbo;
	D3D11_BUFFER_DESC bd;
	ID3D11Buffer* pConstantBuffer;
	Effect effect;

	XMMATRIX matWorld = XMMatrixIdentity();


	Shader vs, ps;

	void initCube() {
		float dx = 1, dy = 1, dz = 1;
		
		XMVECTOR n0{ 0.0f, 0.0f, -1.0f }; // devant
		XMVECTOR n1{0.0f, 0.0f, 1.0f	}; // arrière
		XMVECTOR n2{0.0f, -1.0f, 0.0f	}; // dessous
		XMVECTOR n3{0.0f, 1.0f, 0.0f	}; // dessus
		XMVECTOR n4{-1.0f, 0.0f, 0.0f	}; // face gauche
		XMVECTOR n5{1.0f, 0.0f, 0.0f	}; // face droite

		XMVECTOR point[8] =
		{
			{-dx / 2, dy / 2, -dz / 2	},
			{dx / 2, dy / 2, -dz / 2	},
			{dx / 2, -dy / 2, -dz / 2	},
			{-dx / 2, -dy / 2, -dz / 2	},
			{-dx / 2, dy / 2, dz / 2	},
			{-dx / 2, -dy / 2, dz / 2	},
			{dx / 2, -dy / 2, dz / 2	},
			{dx / 2, dy / 2, dz / 2		}
		};

		std::vector<Vertex> vertices =
		{
			{point[0], n0},	{point[1], n0},	{point[2], n0},
			{point[3], n0},	{point[4], n1},	{point[5], n1},
			{point[6], n1},	{point[7], n1},	{point[3], n2},
			{point[2], n2},	{point[6], n2}, {point[5], n2},
			{point[0], n3},	{point[4], n3},	{point[7], n3},
			{point[1], n3},	{point[0], n4},	{point[3], n4},
			{point[5], n4},	{point[4], n4},	{point[1], n5},
			{point[7], n5},	{point[6], n5},	{point[2], n5},
		};
		std::vector<uint16_t> indices
		{
			0,1,2, // devant 
			0,2,3, // devant 

			5,6,7, // arrière 
			5,7,4, // arrière

			8,9,10, // dessous
			8,10,11, // dessous 

			13,14,15, // dessus 
			13,15,12, // dessus 

			19,16,17, // gauche 
			19,17,18, // gauche 

			20,21,22, // droite 
			20,22,23 // droite 
		};

		vbo = VertexBuffer(device, context, vertices);
		ibo = IndexBuffer(device, context, indices);
		//---------- Shaders
		
		effect = Effect(device, context);
		effect.loadEffectFromFile("res/effects/MiniPhong.fx");
		/*
		vs = Shader(device, context);
		ps = Shader(device, context);
		vs.loadShaderFromFile("res/shaders/VS1.hlsl", Shader::ShaderType::VertexShader);
		ps.loadShaderFromFile("res/shaders/PS1.hlsl", Shader::ShaderType::PixelShader);
		*/

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
		vbo.bind();
		ibo.bind();
		context->IASetInputLayout(effect.m_vertexLayout);
		//vs.bind();
		//ps.bind();
		// Initialiser et sélectionner les « constantes » du VS
		const XMMATRIX viewProj = camera.getVPMatrix();
		const XMMATRIX matWorldViewProj = XMMatrixTranspose(matWorld * viewProj);

		ShadersParams sp;
		sp.matWorldViewProj = matWorldViewProj;
		sp.matWorld = matWorld;

		sp.vLumiere = XMVectorSet(-10.0f, 10.0f, -10.0f, 1.0f); 
		sp.vCamera = XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f); 
		sp.vAEcl = XMVectorSet(1.f, 0.2f, 0.2f, 1.0f); // WHAT ?
		sp.vAMat = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f); 
		sp.vDEcl = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f); 
		sp.vDMat = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);
		context->UpdateSubresource(pConstantBuffer, 0, nullptr, &sp, 0, 0);

		ID3DX11EffectConstantBuffer* pCB = effect.m_effect->GetConstantBufferByName("param");
		pCB->SetConstantBuffer(pConstantBuffer); // **** Rendu de l’objet
		effect.m_pass->Apply(0, context);

		// -- Draw call
		context->DrawIndexed(ibo.getBufferSize(), 0, 0);
	}
	

	virtual void renderMesh(Camera& camera, float deltaTime = 0.f) override {
		static bool b = false;
		if (!b)
		{
			initCube(); b = true;
		}
		updateCube(deltaTime);
		renderCube(camera);
	
	}

	float rotation = 0.F;
	void updateCube(float deltaTime = 0) {

		rotation = rotation + ((XM_PI * 2.0f) / 3.0f * deltaTime);
		matWorld = XMMatrixRotationX(rotation);
		matWorld *= XMMatrixRotationY(rotation);
	}


};