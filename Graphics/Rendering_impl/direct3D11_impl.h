#pragma once

#include "../../vendor/stdafx.h"
#include "../Renderer.h"
#include "../../Platform/WindowsEngine.h"

#include "../../Debug.h"
#include "../../Utils/AABB.h"
#include "../abstraction/Vertex.h"
#include "../abstraction/VertexBuffer.h"
#include "../abstraction/Texture.h"
#include "../abstraction/Effect.h"
#include "../abstraction/Camera.h"
#include "../World/Material.h"

#include "../World/Managers/MeshManager.h"
#include "../World/Cube.h"

#include "../abstraction/2D/TextRenderer.h"

#include <vector>

struct direct3D11_impl : public Renderer::_Impl {

private:
	struct worldParams {
		// la matrice totale 
		XMMATRIX viewProj;
		XMVECTOR lightPos; // la position de la source d’éclairage (Point)
		XMVECTOR cameraPos; // la position de la caméra 
		XMVECTOR ambiantLight; // la valeur ambiante de l’éclairage
		XMVECTOR diffuseLight; // la valeur ambiante du matériau 
		XMVECTOR ambiantMat; // la valeur diffuse de l’éclairage 
		XMVECTOR diffuseMat; // la valeur diffuse du matériau 
	};
	Effect pbrMeshEffect, debugLine, baseMesh, blit;

	size_t verticesCount = 0;
	size_t triangleCount = 0;
	TextRenderer m_textRenderer;


public:

	direct3D11_impl();


private:

	// D3D11 CONTEXT
	ID3D11Device* device = nullptr; // Used to create objects on the GPU
	ID3D11DeviceContext* context = nullptr; // Issues rendering command + actual drawing
	IDXGISwapChain* swapChain = nullptr; // Flips buffers
	ID3D11RenderTargetView* rtv = nullptr; // FrameBuffer
	ID3D11DepthStencilView* depth = nullptr;


private:

	// ~~~~~~~~~~ CORE

	virtual void clearScreen() override final;
	virtual void clearText() override final;
	virtual void setBackbufferToDefault() override final;
	virtual void setDepthBuffer(ID3D11DepthStencilView* other) override final;
	virtual void clearScreen(float r, float g, float b, float a) override final;
	virtual void drawIndexed(size_t count, uint32_t startIndexLocation, uint32_t baseVertexLocation) override;
	virtual void draw(size_t count) override;


	// ~~~~~~~~~~ RENDER

	virtual void renderMesh(Camera& camera, const Mesh& mesh) override;
	virtual void renderMesh(Camera& camera, const Mesh& mesh, const Effect& effect) override;
	virtual void renderPBRMesh(Camera& camera, const Mesh& mesh, const Material& mat) override;
	virtual void renderCubemap(Camera& camera, const Mesh& mesh, const Effect& effect) override;
	virtual void blitTexture(const Texture&, const DirectX::XMVECTOR& colorShift = {1,1,1,1}) override;
	virtual void writeTextOnScreen(const std::string& text, int screenX, int screenY, float scale) override;
	virtual void renderText() override ;

	// ~~~~~~~~~~ DEBUG

	virtual void renderAABB(Camera& cam, const AABB& box) override;
	virtual void renderDebugLine(Camera& cam, Vec from, Vec to) override;
	virtual void renderDebugPerspectiveCameraOutline(Camera& viewCamera, const Camera& outlinedCamera) override;
	virtual void showImGuiDebugData() override;


};