#pragma once

#include "Scene.h"
#include "Renderer.h"

#include "abstraction/abstraction_core.h"


class MainMenu : public Scene {
private:

	Texture m_background{ "res/textures/Menu_screen.dds" };
	Texture m_logo{ "res/textures/logo.dds" };

public:

	MainMenu()
	{
		UIRenderer::attachMouse(wMouse.get());
	}


	virtual void onUpdate(float deltaTime) override {}

	virtual void onRender() override {

		const auto winSize = WindowsEngine::getInstance().getGraphics().getWinSize();
		const auto size = 500;
		Renderer::clearScreen(0, 0, 0, 1);

		Renderer::blitTexture(m_background, DirectX::XMVECTOR{ 1.f,1.f,1.f,0.8f });


		Renderer::startBatching2D();
		Renderer::renderQuadOnScreen(DirectX::XMVECTOR{ (winSize.first/2.f) - size/2.F , -100 }, DirectX::XMVECTOR{ size,size }, m_logo, DirectX::XMVECTOR{});
		Renderer::endBatching2D();
		Renderer::renderBatch2D();

		Renderer::renderText();
	}

	virtual void onImGuiRender() override
	{
		const auto size = 500;
		const auto winSize = WindowsEngine::getInstance().getGraphics().getWinSize();
		UIRenderer::clear();
		if (UIRenderer::Button((winSize.first / 2.f)-150, 550, 300, 100))
		{
			SceneManager::switchToScene("Terrain");
		}

		wMouse->clearPress();

		UIRenderer::renderUI();

	}


};
