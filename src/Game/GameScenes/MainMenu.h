#pragma once

#include "Scene.h"
#include "Renderer.h"

#include "abstraction/abstraction_core.h"


class MainMenu : public Scene {
private:

	Texture m_background;

public:

	MainMenu()
	{
		UIRenderer::attachMouse(wMouse.get());
	}


	virtual void onUpdate(float deltaTime) override {}

	virtual void onRender() override {

		Renderer::clearScreen(0, 0, 0, 1);
	}

	virtual void onImGuiRender() override
	{
		UIRenderer::clear();


		if (UIRenderer::Button(200,200,400,400))
		{
			SceneManager::switchToScene("Terrain");
		}

		UIRenderer::renderUI();


	}


};
