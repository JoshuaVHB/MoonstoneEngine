#pragma once

#include "Scene.h"
#include "Renderer.h"
#include "abstraction/Texture.h"
#include "abstraction/2D/Renderer2D.h"
#include "abstraction/2D/UI/UIRenderer.h"

#define _XM_NO_INTRINSICS_
#include <directXmath.h>

class UITestScene : public Scene {
private:

	Texture m_bb{ "res/textures/breadbug.dds" };
	FrameBuffer m_fbo;
	Texture screenShot;
	bool m_isPaused = false;
	bool m_hasEscBeenReleased = false;
	float elapsed = 0;

	Renderer2D m_dd;
	
public:

	UITestScene() {}


	virtual void onUpdate(float deltaTime) override
	{
		if (wMouse->isLeftPressed()) std::cout << "HELLO" << std::endl;
		if (!wKbd->isKeyPressed(VK_ESCAPE)) m_hasEscBeenReleased = true;
		if (wKbd->isKeyPressed(VK_ESCAPE) && m_hasEscBeenReleased)
		{
			m_hasEscBeenReleased = false;
			m_isPaused = !m_isPaused;
		}

		if (m_isPaused) return;

		

		elapsed += deltaTime;
		UIRenderer::attachMouse(wMouse.get());
	}

	virtual void onRender() override {

		if (m_isPaused) m_fbo.bind();
		Renderer::clearScreen(0, 0, 0, 1);
		Renderer::blitTexture(m_bb, DirectX::XMVECTOR{1,1,1,1});
		Renderer::writeTextOnScreen(std::to_string(elapsed), -500, -200, 1);

		

		m_dd.BeginBatch();

		if (UIRenderer::Button(0,0,400,400))
		{
			std::cout << "Bonjour je suis le bouton" << std::endl;
		}



		m_dd.EndBatch();
		m_dd.Flush();

		if (m_isPaused)
		{
			m_fbo.unbind();
			Renderer::setBackbufferToDefault();
			screenShot = Texture(m_fbo.getResource(0));
			Renderer::blitTexture(screenShot, DirectX::XMVECTOR{1,1,1,0.5});
			Renderer::writeTextOnScreen("Pause", 0, 0, 1);
		}
		 
		Renderer::renderText();
		UIRenderer::renderUI();

	}

	virtual void onImGuiRender() override
	{
		ImGui::Text("LP : %d, LD : %d", wMouse->isLeftPressed(), wMouse->isLeftDown());
		ImGui::Text("RP : %d, RD : %d", wMouse->isRightPressed(), wMouse->isRightDown());
		wMouse->clearPress();

	}


};
