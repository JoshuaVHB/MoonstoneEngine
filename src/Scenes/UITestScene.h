#pragma once

#include "Scene.h"
#include "Renderer.h"
#include "abstraction/Texture.h"

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

public:

	UITestScene() {}


	virtual void onUpdate(float deltaTime) override
	{




		if (!wKbd->isKeyPressed(VK_ESCAPE)) m_hasEscBeenReleased = true;
		if (wKbd->isKeyPressed(VK_ESCAPE) && m_hasEscBeenReleased)
		{
			m_hasEscBeenReleased = false;
			m_isPaused = !m_isPaused;
		}

		if (m_isPaused) return;

		elapsed += deltaTime;
	}

	virtual void onRender() override {

		if (m_isPaused) m_fbo.bind();
		Renderer::clearScreen(0, 0, 0, 1);
		Renderer::blitTexture(m_bb, DirectX::XMVECTOR{1,1,1,1});
		Renderer::writeTextOnScreen(std::to_string(elapsed), -500, -200, 1);

		if (m_isPaused)
		{
			m_fbo.unbind();
			Renderer::setBackbufferToDefault();
			screenShot = Texture(m_fbo.getResource(0));
			Renderer::blitTexture(screenShot, DirectX::XMVECTOR{1,1,1,0.5});
			Renderer::writeTextOnScreen("Pause", 0, 0, 1);
		}
		Renderer::renderText();

	}

	virtual void onImGuiRender() override {}


};
