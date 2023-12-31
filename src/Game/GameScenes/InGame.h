#pragma once

#include "Scene.h"
#include "Renderer.h"
#include "../Cloporte.h"
#include "abstraction/DeferredRenderer.h"

#include "abstraction/2D/TextRenderer.h"
#include "abstraction/2D/UI/UIRenderer.h"
#include "World/WorldRendering/Skybox.h"
#include "World/Player.h"

#define PRINT_VECTOR(name, vec) ImGui::Text("%s\n  X : %f\n  Y : %f\n  Z : %f",\
	name,\
	XMVectorGetX(vec),\
	XMVectorGetY(vec),\
	XMVectorGetZ(vec)\
);

struct worldParams {
	XMMATRIX viewProj;
	XMVECTOR lightPos;
	XMVECTOR cameraPos;
	XMVECTOR sunColor = { 1.f,1.f,.8f,1.f };
	XMVECTOR sunStrength = { 0.75f };
};

class InGame : public Scene {
private:

	Skybox m_skybox{};
	Player m_playerCam;
	Cloporte m_player{};
	worldParams sp;
	bool lockInputs = false;
	AABB Endcheck;

	TextRenderer m_textRenderer;

	DeferredRenderer m_renderer;
	Effect	m_baseMeshEffect;
	Texture m_rockTexture{ L"res/textures/rock.dds" };
	Texture m_grassTexture{ L"res/textures/seamless.dds" };
	Texture m_grass6Texture{ L"res/textures/breadbug.dds" };
	Texture m_snow{ L"res/textures/snow.dds" };

	UIRenderer m_ui;

	struct ui_Button
	{
		int x, y;
		int width, height;

		bool isClicked();

	};

	bool tmp = false;

	bool deferredRendering = false;

public:


	InGame()
	{

		Endcheck = AABB::makeAABBFromPoints({ 20,0,20 }, { 100,100,100 });
		m_baseMeshEffect.loadEffectFromFile("res/effects/baseMesh.fx");


		// -- Setup the effect correctly
		m_baseMeshEffect.addNewCBuffer("worldParams", sizeof(worldParams)); // For camera, light, colors...
		m_baseMeshEffect.addNewCBuffer("meshParams", sizeof(meshParams)); // For model matrix basically



		// -- Specify how the input vertices are layered
		InputLayout testlayout;
		testlayout.pushBack<3>(InputLayout::Semantic::Position);
		testlayout.pushBack<3>(InputLayout::Semantic::Normal);
		testlayout.pushBack<2>(InputLayout::Semantic::Texcoord);
		m_baseMeshEffect.bindInputLayout(testlayout);
		m_ui.attachMouse(wMouse.get());



		//m_boule = MeshManager::loadMeshFromFile("res/mesh/Game/boule.obj");
		//m_boule.getBoundingBox() = BoundingSphere()
	}

	virtual void onUpdate(float deltaTime) override
	{
		Camera& currentCam = m_player.getCurrentCamera();

		m_player.update(deltaTime);

		m_renderer.update(currentCam);

		sp.viewProj = XMMatrixTranspose(currentCam.getVPMatrix());
		sp.lightPos = XMVectorSet(-100.0f, 1000.f, -1000.0f, 1.0f); // sun ?
		sp.cameraPos = currentCam.getPosition();
		m_baseMeshEffect.updateSubresource(sp, "worldParams");
		m_baseMeshEffect.sendCBufferToGPU("worldParams");

	}
	
	void renderSceneFn()
	{
		Camera& currentCam = m_player.getCurrentCamera();

		m_renderer.renderMesh(currentCam, m_player.getMesh());
		m_renderer.renderSkybox(currentCam, m_skybox);
	}
	 


	virtual void onRender() override {

		Renderer::clearScreen();
		m_textRenderer.clear();

		Camera& currentCam = m_player.getCurrentCamera();

		if(deferredRendering)
		{
			m_renderer.clear();
			m_renderer.renderDeferred([&]() {renderSceneFn(); }, currentCam);
		}
		else
		{
				
			Renderer::renderMesh(currentCam, m_player.getMesh(), m_baseMeshEffect);
			m_skybox.renderSkybox(currentCam);
			Renderer::renderAABB(currentCam, Endcheck);

		}
		if (SphereAABBTest(m_player.getBoundingSphere(), Endcheck))
		{
			m_textRenderer.writeTextOnScreen("GG !!!!", 0, 0, 3);
		}

		if (tmp) m_textRenderer.writeTextOnScreen("You have pressed on the button", 100, 100, 1);
		m_textRenderer.render();
	}

	virtual void onImGuiRender() override
	{
		m_renderer.showDebugWindow();
		ImGui::Begin("Player control panel");
		if (ImGui::Checkbox("Lock Inputs", &lockInputs))
		{
			m_playerCam.lockInputs(lockInputs);
		}

		

		if (ImGui::Button("Switch camera view mode"))
		{
			m_player.switchView();
		}

		if (UIRenderer::Button(0, 0, 400, 400))
		{
			tmp = !tmp;
		}


		ImGui::Checkbox("Render deferred", &deferredRendering);
		ImGui::Text("Has pressed the button ? %d", UIRenderer::Button(0, 0, 400, 400));
		ImGui::Text("Has collided ? %d", SphereAABBTest(m_player.getBoundingSphere(), Endcheck));

		//m_player.getMesh().getTransform().showControlWindow();
		auto v = m_player.getForward();
		auto c = m_player.getCurrentCamera().getPosition();
		PRINT_VECTOR("Player forward",v)
		PRINT_VECTOR("Campos",c)
		ImGui::End();
		m_ui.renderUI();
	}

};
