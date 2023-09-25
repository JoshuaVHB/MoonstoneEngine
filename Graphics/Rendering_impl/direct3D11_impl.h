#pragma once

#include "../Renderer.h"
#include "../../Platform/WindowsEngine.h"

#include "../../Debug.h"

#include <d3d11.h>
#include <directXmath.h>
#include "../../vendor/stdafx.h"

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
	
	//virtual void renderMesh(const Mesh& mesh) override{	}
	ID3D11PixelShader* pPixelShader;
	ID3D11Buffer* pVertexBuffer;
	ID3D11Buffer* pIndexBuffer;
	D3D11_SUBRESOURCE_DATA InitData;
	D3D11_BUFFER_DESC bd;
	ID3D11VertexShader* pVertexShader;
	ID3DBlob* pVSBlob = nullptr;
	ID3DBlob* pPSBlob = nullptr;
	ID3D11InputLayout** pVertexLayout = nullptr;
	ID3D11Buffer* pConstantBuffer;

	XMMATRIX matWorld = XMMatrixIdentity();

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{ 
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	uint16_t index_bloc[36]{

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

	void initCube() {
		float dx = 1, dy = 1, dz = 1;
		
		XMFLOAT3 n0(0.0f, 0.0f, -1.0f); // devant
		XMFLOAT3 n1(0.0f, 0.0f, 1.0f); // arrière
		XMFLOAT3 n2(0.0f, -1.0f, 0.0f); // dessous
		XMFLOAT3 n3(0.0f, 1.0f, 0.0f); // dessus
		XMFLOAT3 n4(-1.0f, 0.0f, 0.0f); // face gauche
		XMFLOAT3 n5(1.0f, 0.0f, 0.0f); // face droite

		XMFLOAT3 point[8] =
		{
			XMFLOAT3(-dx / 2, dy / 2, -dz / 2),
			XMFLOAT3(dx / 2, dy / 2, -dz / 2),
			XMFLOAT3(dx / 2, -dy / 2, -dz / 2),
			XMFLOAT3(-dx / 2, -dy / 2, -dz / 2),
			XMFLOAT3(-dx / 2, dy / 2, dz / 2),
			XMFLOAT3(-dx / 2, -dy / 2, dz / 2),
			XMFLOAT3(dx / 2, -dy / 2, dz / 2),
			XMFLOAT3(dx / 2, dy / 2, dz / 2)
		};

		Vertex sommets[24]; // Le devant du bloc 
		sommets[0] = Vertex(point[0], n0);
		sommets[1] = Vertex(point[1], n0);
		sommets[2] = Vertex(point[2], n0);
		sommets[3] = Vertex(point[3], n0);
		// L’arrière du bloc 
		sommets[4] = Vertex(point[4], n1);
		sommets[5] = Vertex(point[5], n1);
		sommets[6] = Vertex(point[6], n1);
		sommets[7] = Vertex(point[7], n1);
		// Le dessous du bloc 
		sommets[8] = Vertex(point[3], n2);
		sommets[9] = Vertex(point[2], n2);
		sommets[10] = Vertex(point[6], n2);
		sommets[11] = Vertex(point[5], n2);
		// Le dessus du bloc 
		sommets[12] = Vertex(point[0], n3);
		sommets[13] = Vertex(point[4], n3);
		sommets[14] = Vertex(point[7], n3);
		sommets[15] = Vertex(point[1], n3);
		// La face gauche 
		sommets[16] = Vertex(point[0], n4);
		sommets[17] = Vertex(point[3], n4);
		sommets[18] = Vertex(point[5], n4);
		sommets[19] = Vertex(point[4], n4);
		// La face droite 
		sommets[20] = Vertex(point[1], n5);
		sommets[21] = Vertex(point[7], n5);
		sommets[22] = Vertex(point[6], n5);
		sommets[23] = Vertex(point[2], n5);




		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(sommets);
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = sommets;
		DX_TRY_CODE(device->CreateBuffer(&bd, &InitData, &pVertexBuffer), -5);


		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(index_bloc);
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = index_bloc;
		DX_TRY(device->CreateBuffer(&bd, &InitData, &pIndexBuffer), -6);

		DX_TRY_CODE(D3DCompileFromFile(L"vs1.hlsl", nullptr, nullptr, "VS1", "vs_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &pVSBlob, nullptr),
			-7);

		DX_TRY_CODE(device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &pVertexShader),
			-8);

		DX_TRY_CODE(device->CreateInputLayout(CSommetBloc::layout, 24, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &pVertexLayout), -9);
		pVSBlob->Release();

		D3D11_BUFFER_DESC bd; ZeroMemory(&bd, sizeof(bd)); 
		bd.Usage = D3D11_USAGE_DEFAULT; 
		bd.ByteWidth = sizeof(matWorld);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		device->CreateBuffer(&bd, nullptr, &pConstantBuffer);


		// Compilation et chargement du pixel shader
		DX_TRY_CODE(D3DCompileFromFile(L" ps1.hlsl", nullptr, nullptr, "PS1", "ps_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &pPSBlob, nullptr),
			-9); 
		DX_TRY_CODE(device->CreatePixelShader(
			pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &pPixelShader), 
			-10);
		pPSBlob->Release(); // On n’a plus besoin du blob

	}

	void renderCube(Camera& camera) {

		// Obtenir le contexte 
		// Choisir la topologie des primitives 
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// Source des sommets 
		const UINT stride = sizeof(Vertex);
		const UINT offset = 0;
		context->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
		// Source des index
		context->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
		// input layout des sommets 
		context->IASetInputLayout(*pVertexLayout);
		// Activer le VS
		context->VSSetShader(pVertexShader, nullptr, 0);
		// Initialiser et sélectionner les « constantes » du VS
		const XMMATRIX viewProj = camera.getVPMatrix();
		const XMMATRIX matWorldViewProj = XMMatrixTranspose(matWorld * viewProj);
		context->UpdateSubresource(pConstantBuffer, 0, nullptr, &matWorldViewProj, 0, 0);
		context->VSSetConstantBuffers(0, 1, &pConstantBuffer);
		// Activer le PS 
		context->PSSetShader(pPixelShader, nullptr, 0);
		// **** Rendu de l’objet
		context->DrawIndexed(36, 0, 0);
	}
	


};