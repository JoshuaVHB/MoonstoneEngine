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

		Renderer::writeTextOnScreen("Bugs Balls", -200, 200, 2);

		Renderer::renderText();
	}

	virtual void onImGuiRender() override
	{
		UIRenderer::clear();

		if (UIRenderer::Button(600,600,300,100))
		{
			if (wMouse->isLeftPressed())
			{
				SceneManager::switchToScene("Terrain");
			}
		}

		wMouse->clearPress();

		UIRenderer::renderUI();


	}


};
