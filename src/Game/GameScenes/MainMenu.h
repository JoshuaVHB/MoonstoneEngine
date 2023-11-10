#pragma once

#include "Scene.h"
#include "Renderer.h"

#include "abstraction/abstraction_core.h"


class MainMenu : public Scene {
private:

	Texture m_background{ "res/textures/Menu_screen.dds" };

public:

	MainMenu()
	{
		UIRenderer::attachMouse(wMouse.get());
	}


	virtual void onUpdate(float deltaTime) override {}

	virtual void onRender() override {

		Renderer::clearScreen(0, 0, 0, 1);


		Renderer::blitTexture(m_background, DirectX::XMVECTOR{ 1,1,1,0.8 });
		Renderer::writeTextOnScreen("Bugs Balls", -200, 200, 1);

		Renderer::renderText();
	}

	virtual void onImGuiRender() override
	{
		UIRenderer::clear();
		std::cout << wMouse->isLeftPressed() << std::endl;
		if (UIRenderer::Button(400, 400, 300, 100))
		{
			SceneManager::switchToScene("Terrain");
		}

		wMouse->clearPress();

		UIRenderer::renderUI();

	}


};
