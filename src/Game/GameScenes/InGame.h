#pragma once

#include "Scene.h"
#include "Renderer.h"
#include "../Cloporte.h"
#include "abstraction/DeferredRenderer.h"

#include "World/WorldRendering/Skybox.h"


class InGame : public Scene {
private:

	Skybox m_skybox{};

	Cloporte m_player{};
	Mesh m_boule;


	DeferredRenderer m_renderer;

public:

	InGame()
	{
		m_boule = MeshManager::loadMeshFromFile("res/mesh/Game/boule.obj");

		Renderer::setBackbufferToDefault();
	}

	virtual void onUpdate(float deltaTime) override
	{
		m_player.update(deltaTime);
		m_renderer.update(m_player.getCurrentCamera());

	}

	void renderSceneFn()
	{
		Camera& currentCam = m_player.getCurrentCamera();

		m_renderer.renderMesh(currentCam, m_player.getMesh());
		m_renderer.renderSkybox(currentCam, m_skybox);
	}


	virtual void onRender() override {

		Renderer::clearScreen(0, 0, 0, 1);
		Camera& currentCam = m_player.getCurrentCamera();
		m_renderer.clear();
		m_renderer.renderDeferred([this]() {
			auto& a = m_player;
			renderSceneFn ();
		}, currentCam);
		Renderer::setBackbufferToDefault();


	}

	virtual void onImGuiRender() override
	{
		m_player.getMesh().getTransform().showControlWindow();
	}


};
