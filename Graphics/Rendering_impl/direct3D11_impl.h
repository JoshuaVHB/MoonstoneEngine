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
	Effect pbrMeshEffect, debugLine;

	size_t verticesCount = 0;
	size_t triangleCount = 0;


public:

	direct3D11_impl() 
	{
		WindowsEngine& engine = WindowsEngine::getInstance();
		d3d11_graphics& gfx = engine.getGraphics();
		device = gfx.getDevice();
		context = gfx.getImmediateContext();
		swapChain = gfx.getSwapChain();
		rtv = gfx.getRenderTargetView();
		depth = gfx.getDepthBuffer().getView();
		
		pbrMeshEffect.loadEffectFromFile("res/effects/pbrMesh.fx");
		

		InputLayout layout, debuglayout;
		layout.pushBack<3>(InputLayout::Semantic::Position);
		layout.pushBack<3>(InputLayout::Semantic::Normal);
		layout.pushBack<2>(InputLayout::Semantic::Texcoord);
		pbrMeshEffect.bindInputLayout(layout);

		pbrMeshEffect.addNewCBuffer("meshParams", sizeof(XMMATRIX));
		pbrMeshEffect.addNewCBuffer("worldParams", sizeof(worldParams));
		pbrMeshEffect.addNewCBuffer("materialParams", sizeof(MaterialCoefs));


		debugLine.loadEffectFromFile("res/effects/debugLine.fx");
		debuglayout.pushBack<3>(InputLayout::Semantic::Position);
		debugLine.bindInputLayout(debuglayout);
		debugLine.addNewCBuffer("worldParams", sizeof(XMMATRIX));
		

	}


private:

	// D3D11 CONTEXT
	ID3D11Device* device = nullptr; // Used to create objects on the GPU
	ID3D11DeviceContext* context = nullptr; // Issues rendering command + actual drawing
	IDXGISwapChain* swapChain = nullptr; // Flips buffers
	ID3D11RenderTargetView* rtv = nullptr; // Framebuffer
	ID3D11DepthStencilView* depth = nullptr;


private:

	virtual void clearScreen() override final {
				
		d3d11_graphics& gfx = WindowsEngine::getInstance().getGraphics();
		gfx.clearDepth();
		gfx.clearFramebuffer();

	}

	virtual void setBackbufferToDefault() override final {

		context->OMSetRenderTargets(1, &rtv, depth);
	}

	virtual void setDepthBuffer(ID3D11DepthStencilView* other) override final {

		depth = other;
	}


	virtual void clearScreen(float r, float g, float b, float a) override final { 
		const FLOAT rgba[4] = {r,g,b,a};
		context->ClearRenderTargetView(rtv, rgba);
	}

	virtual void drawIndexed(size_t count, uint32_t startIndexLocation, uint32_t baseVertexLocation) override {

		context->DrawIndexed(static_cast<UINT>(count), startIndexLocation, baseVertexLocation);
	}

	virtual void draw(size_t count) override {

		context->Draw(static_cast<UINT>(count), 0);
	}


	virtual void renderMesh(Camera& camera, const Mesh& mesh, const Effect& effect) override {

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetInputLayout(effect.getVertexLayout());

		effect.updateSubresource(XMMatrixTranspose(mesh.getTransform().getTransformationMatrix()), "meshParams"); // TODO make this more flexible
		effect.sendCBufferToGPU("meshParams");

		effect.apply();
		mesh.draw();
		effect.unbindResources();

		triangleCount += mesh.getTriangleCount();

	}


	virtual void renderPBRMesh(Camera& camera, const Mesh& mesh, const Material& mat) override {

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetInputLayout(pbrMeshEffect.getVertexLayout());

		worldParams sp;

		sp.viewProj = XMMatrixTranspose(camera.getVPMatrix());
		sp.lightPos = XMVectorSet(-10.0f , 10.0f , -10.0f, 1.0f);
		sp.cameraPos = camera.getPosition();
		sp.ambiantLight = XMVectorSet(.2f, 0.2f, 0.2f, 1.0f);
		sp.diffuseLight = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		sp.ambiantMat = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		sp.diffuseMat = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);

		pbrMeshEffect.updateSubresource(sp, "worldParams");
		pbrMeshEffect.sendCBufferToGPU("worldParams");

		pbrMeshEffect.updateSubresource(XMMatrixTranspose(mesh.getTransform().getTransformationMatrix()), "meshParams"); // TODO make this more flexible
		pbrMeshEffect.sendCBufferToGPU("meshParams");


		//pbrMeshEffect.updateSubresource(mesh.getMaterials()[0].getCoefficients(), "materialParams");
		//pbrMeshEffect.sendCBufferToGPU("materialParams");


		pbrMeshEffect.bindTexture("albedo", mat.queryTexture<TextureType::ALBEDO>());


		pbrMeshEffect.apply();
		mesh.draw();
	}

	// todo make this more clear
	virtual void renderCubemap(Camera& camera, const Mesh& mesh, const Effect& effect) override {

		
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetInputLayout(effect.getVertexLayout());
		effect.apply();
		mesh.draw();


	}

	

	virtual Mesh loadMeshFromFile(const std::filesystem::path& path) override {
		return MeshManager::loadMeshFromFile(path);
	}

	virtual void renderAABB(Camera& cam, const AABB& box) override
	{
		Vec o = box.origin;
		Vec x = { XMVectorGetX(box.size), 0, 0 };
		Vec y = { 0, XMVectorGetY(box.size), 0 };
		Vec z = { 0, 0, XMVectorGetZ(box.size) };

		renderDebugLine(cam, o, o + x);
		renderDebugLine(cam, o + y, o + x + y);
		renderDebugLine(cam, o + z, o + x + z);
		renderDebugLine(cam, o + y + z, o + x + y + z);
		renderDebugLine(cam, o, o + y);
		renderDebugLine(cam, o + x, o + y + x);
		renderDebugLine(cam, o + z, o + y + z);
		renderDebugLine(cam, o + x + z, o + y + x + z);
		renderDebugLine(cam, o, o + z);
		renderDebugLine(cam, o + x, o + z + x);
		renderDebugLine(cam, o + y, o + z + y);
		renderDebugLine(cam, o + x + y, o + z + x + y);
	}

	virtual void renderDebugLine(Camera& cam, Vec from, Vec to) override {
				
		setBackbufferToDefault();
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		context->IASetInputLayout(debugLine.getVertexLayout());


		debugLine.updateSubresource(cam.getVPMatrix(), "worldParams");
		debugLine.sendCBufferToGPU("worldParams");

		debugLine.apply();


		VertexBuffer vbo({ Vertex{from}, Vertex{to} });
		vbo.bind();

		context->Draw(2, 0);


	}

	virtual void renderDebugPerspectiveCameraOutline(Camera& viewCamera, const Camera& outlinedCamera) override
	{
		//const PerspectiveProjection& proj = outlinedCamera.getProjection<PerspectiveProjection>();
		//PerspectiveProjection proj = PerspectiveProjection();


		float zfar = 1000.F;
		float znear = 0.10F;
		float fovy = 3.1415f / 4.F;
		float aspect = 2.0f;

		Vec pos = outlinedCamera.getPosition();
		Vec I = outlinedCamera.getHorizontalDir();
		Vec J = outlinedCamera.getUp();
		Vec F = outlinedCamera.getForward();
		float dh = sin(fovy * .5f);
		float dw = dh * aspect;
		Vec U1 = F + dh * J + dw * I;
		Vec U2 = F - dh * J + dw * I;
		Vec U3 = F - dh * J - dw * I;
		Vec U4 = F + dh * J - dw * I;
		float zNear = -znear;
		float zFar = -zfar;

		renderDebugLine(viewCamera, pos, pos + F * zFar		); // dir
		renderDebugLine(viewCamera, pos, pos + Vec{0,1,0,1}); // world up
		renderDebugLine(viewCamera, pos, pos + I		); // right
		renderDebugLine(viewCamera, pos, pos + J			); // up
		renderDebugLine(viewCamera, pos, pos + U1 * zFar		);
		renderDebugLine(viewCamera, pos, pos + U2 * zFar		);
		renderDebugLine(viewCamera, pos, pos + U3 * zFar		);
		renderDebugLine(viewCamera, pos, pos + U4 * zFar		);
		renderDebugLine(viewCamera, pos + U1 * zFar, pos + U2 * zFar);
		renderDebugLine(viewCamera, pos + U2 * zFar, pos + U3 * zFar);
		renderDebugLine(viewCamera, pos + U3 * zFar, pos + U4 * zFar);
		renderDebugLine(viewCamera, pos + U4 * zFar, pos + U1 * zFar);

	}

	virtual void showImGuiDebugData() override
	{
		ImGui::Begin("D3D11 Rendering debug data:");
		ImGui::Text("Current drawn triangle count : %d", triangleCount);
		ImGui::End();

		triangleCount = 0;

	}

};