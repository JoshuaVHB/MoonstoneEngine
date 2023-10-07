#pragma once

#include "../abstraction/Camera.h"

#include "../../Platform/WindowsEngine.h"
#include <optional>


// This is the default FPS player.
// Windows specific

extern std::unique_ptr<Mouse> wMouse;
extern std::unique_ptr<Keyboard> wKbd; 

class Player 
{
private:

	Camera		m_camera;
	XMFLOAT3	m_position;
	
	Vec delta;

	const Vec2 winSize = WindowsEngine::getInstance().getGraphics().getWinSize();
	HWND hWnd = WindowsEngine::getInstance().getHwnd();

	bool isConfined = false;

public:

	Player()
	{
		m_camera.setProjection<PerspectiveProjection>(PerspectiveProjection());
	}

	Camera& getCamera() { return m_camera; }

	void step(float deltaTime)
	{
		handleMouseEvents();		
		handleKeyboardEvents();

		m_camera.updateCam();

	}

	void onImGuiRender() 
	{
		ImGui::Begin("Player debug");

		ImGui::SliderAngle("Yaw", &m_camera.getRawYaw());
		ImGui::SliderAngle("Pitch", &m_camera.getRawPitch());

		ImGui::End();

	}

private:


	void handleKeyboardEvents() {
		Vec camForward = m_camera.getForwardDir();
		Vec camHorizontal = m_camera.getHorizontalDir();

		Keyboard::Event e = wKbd->readKey();

		if (wKbd->isKeyPressed(VK_SPACE)) {
			delta = XMVectorSetY(delta, XMVectorGetY(delta) + 0.5f);
		}
		else if (wKbd->isKeyPressed(VK_SHIFT)) {
			delta = XMVectorSetY(delta, XMVectorGetY(delta) - 0.5f);
		}

		if (wKbd->isKeyPressed(Keyboard::letterCodeFromChar('q'))) {
			delta = XMVectorSetZ(delta, XMVectorGetZ(delta) - XMVectorGetZ(camHorizontal));
			delta = XMVectorSetX(delta, XMVectorGetX(delta) - XMVectorGetX(camHorizontal));
		}

		else if (wKbd->isKeyPressed(Keyboard::letterCodeFromChar('d'))) {
			delta = XMVectorSetZ(delta, XMVectorGetZ(delta) + XMVectorGetZ(camHorizontal));
			delta = XMVectorSetX(delta, XMVectorGetX(delta) + XMVectorGetX(camHorizontal));
		}

		if (wKbd->isKeyPressed(Keyboard::letterCodeFromChar('z'))) {
			delta = XMVectorSetZ(delta, XMVectorGetZ(delta) - XMVectorGetZ(camForward));
			delta = XMVectorSetX(delta, XMVectorGetX(delta) - XMVectorGetX(camForward));
		}

		else if (wKbd->isKeyPressed(Keyboard::letterCodeFromChar('s'))) {
			delta = XMVectorSetZ(delta, XMVectorGetZ(delta) + XMVectorGetZ(camForward));
			delta = XMVectorSetX(delta, XMVectorGetX(delta) + XMVectorGetX(camForward));
		}


		m_camera.setPosition(delta);
	}

	void handleMouseEvents() {
		Mouse::Event me;

		while ((me = wMouse->read()).isValid())
		{
			if (me.getType() == Mouse::Event::Type::RPRESS) {

				if (isConfined) {
					wMouse->freeCursor();
					wMouse->enableCursor();

				}
				else {
					wMouse->confineCursor(hWnd);
					wMouse->disableCursor();
				}

				isConfined = !isConfined;
			}
		}

		wMouse->flush();
		float dx = 0, dy = 0;
		while (const auto rawDelta = wMouse->readRawdelta()) {

			if (isConfined) {
				dx += rawDelta->x;
				dy += rawDelta->y;
			}
		}

		float m_dx = (float)dx / (float)winSize.x * DirectX::XM_PI;
		float m_dy = (float)dy / (float)winSize.y * DirectX::XM_PI;
		m_camera.rotate(m_dx, m_dy);

	}

};