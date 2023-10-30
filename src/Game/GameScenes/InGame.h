#pragma once

#include "Scene.h"
#include "Renderer.h"
#include "../Cloporte.h"
#include "abstraction/DeferredRenderer.h"

#include "abstraction/2D/TextRenderer.h"
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



		//m_boule = MeshManager::loadMeshFromFile("res/mesh/Game/boule.obj");
		//m_boule.getBoundingBox() = BoundingSphere()
	}

	virtual void onUpdate(float deltaTime) override
	{
		Camera& currentCam = m_playerCam.getCamera();

		if (lockInputs)
			m_player.update(deltaTime);
		m_playerCam.step(deltaTime);
		//m_renderer.update(m_player.getCurrentCamera());

		sp.viewProj = XMMatrixTranspose(currentCam.getVPMatrix());
		sp.lightPos = XMVectorSet(-100.0f, 1000.f, -1000.0f, 1.0f); // sun ?
		sp.cameraPos = currentCam.getPosition();
		m_baseMeshEffect.updateSubresource(sp, "worldParams");
		m_baseMeshEffect.sendCBufferToGPU("worldParams");




	}
	/*
	void renderSceneFn()
	{
		Camera& currentCam = m_player.getCurrentCamera();

		m_renderer.renderMesh(currentCam, m_player.getMesh());
		m_renderer.renderSkybox(currentCam, m_skybox);
	}
	 */


	virtual void onRender() override {

		FrameBuffer::unbind();
		Renderer::clearScreen();
		m_textRenderer.clear();

		//Camera& currentCam = m_playerCam.getCamera(); //m_player.getCurrentCamera();
		Camera& currentCam = m_player.getCurrentCamera(); //m_player.getCurrentCamera();

		m_baseMeshEffect.bindTexture("tex", m_grassTexture.getTexture());
		Renderer::renderMesh(currentCam, m_player.getMesh(), m_baseMeshEffect);
		m_skybox.renderSkybox(currentCam);

		/*
		 *
		m_renderer.clear();
		m_renderer.renderDeferred([this]() {
			auto& a = m_player;
			renderSceneFn ();
		}, currentCam);
		*/
		//Renderer::setBackbufferToDefault();
		Renderer::renderAABB(currentCam, Endcheck);
		if (SphereAABBTest(m_player.getBoundingSphere(), Endcheck))
		{
			m_textRenderer.writeTextOnScreen("GG !!!!", 0, 0, 3);
		}
		
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

		ImGui::Text("Has collided ? %d", SphereAABBTest(m_player.getBoundingSphere(), Endcheck));

		//m_player.getMesh().getTransform().showControlWindow();
		//auto v = m_player.getForward();
		//auto c = m_player.getCurrentCamera().getPosition();
		//PRINT_VECTOR("Player forward",v)
		//PRINT_VECTOR("Campos",c)
		ImGui::End();
	}


};
