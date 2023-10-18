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

		Vec forwardDir = XMVector3Normalize(Vec{ XMVectorGetX(camForward), 0, XMVectorGetZ(camForward) });
		Vec horizontalDir = XMVector3Normalize(Vec{ XMVectorGetX(camHorizontal), 0, XMVectorGetZ(camHorizontal) });

		Keyboard::Event e = wKbd->readKey();

		if (wKbd->isKeyPressed(VK_SPACE)) {
			delta = XMVectorSetY(delta, XMVectorGetY(delta) + 0.5f);
		}
		else if (wKbd->isKeyPressed(VK_SHIFT)) {
			delta = XMVectorSetY(delta, XMVectorGetY(delta) - 0.5f);
		}

		if ( wKbd->isKeyPressed(Keyboard::letterCodeFromChar('q')) || wKbd->isKeyPressed(Keyboard::letterCodeFromChar('a'))) {
			delta = XMVectorSetZ(delta, XMVectorGetZ(delta) - XMVectorGetZ(horizontalDir));
			delta = XMVectorSetX(delta, XMVectorGetX(delta) - XMVectorGetX(horizontalDir));
		}

		else if (wKbd->isKeyPressed(Keyboard::letterCodeFromChar('d'))) {
			delta = XMVectorSetZ(delta, XMVectorGetZ(delta) + XMVectorGetZ(horizontalDir));
			delta = XMVectorSetX(delta, XMVectorGetX(delta) + XMVectorGetX(horizontalDir));
		}

		if (wKbd->isKeyPressed(Keyboard::letterCodeFromChar('z')) || wKbd->isKeyPressed(Keyboard::letterCodeFromChar('w')))
		{
			delta = XMVectorSetZ(delta, XMVectorGetZ(delta) - XMVectorGetZ(forwardDir));
			delta = XMVectorSetX(delta, XMVectorGetX(delta) - XMVectorGetX(forwardDir));
		}

		else if (wKbd->isKeyPressed(Keyboard::letterCodeFromChar('s'))) {
			delta = XMVectorSetZ(delta, XMVectorGetZ(delta) + XMVectorGetZ(forwardDir));
			delta = XMVectorSetX(delta, XMVectorGetX(delta) + XMVectorGetX(forwardDir));
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