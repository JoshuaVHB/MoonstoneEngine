#pragma once

#include "Scene.h"
#include "Renderer.h"
#include "../Cloporte.h"

#include "World/WorldRendering/Skybox.h"


class InGame : public Scene {
private:

	Skybox m_skybox;
	Cloporte m_player;

	float r;

public:

	InGame() {}


	virtual void onUpdate(float deltaTime) override {}

	virtual void onRender() override {

		Renderer::clearScreen(0, 0, 0, 1);

		Camera& currentCam = m_player.getCurrentCamera();
		m_skybox.renderSkybox(currentCam);


	}

	virtual void onImGuiRender() override {}


};
