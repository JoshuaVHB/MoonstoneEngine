#pragma once

#include <optional>
#include <DirectXMath.h>
#include "../abstraction/Camera.h"

#include "../../Platform/WindowsEngine.h"
#include "../../Utils/Vec2.h"

using namespace DirectX;
// This is the default FPS player.
// Windows specific

extern std::unique_ptr<Mouse> wMouse;
extern std::unique_ptr<Keyboard> wKbd; 

class Player 
{
private:

	Camera		m_camera;
	XMFLOAT3	m_position;
	
	XMVECTOR delta;

	const Vec2<int> winSize = Vec2<int>(WindowsEngine::getInstance().getGraphics().getWinSize());
	HWND hWnd = WindowsEngine::getInstance().getHwnd();

	bool isConfined = false;

public:

	Player()
	{
		m_position = { 0,0,0 };
		delta = { 0,0,0 };
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
		const XMVECTOR camForward = m_camera.getForwardDir();
		const XMVECTOR camHorizontal = m_camera.getHorizontalDir();
		const XMVECTOR forwardDir = XMVector3Normalize(XMVECTOR{ XMVectorGetX(camForward), 0, XMVectorGetZ(camForward) });
		const XMVECTOR horizontalDir = XMVector3Normalize(XMVECTOR{ XMVectorGetX(camHorizontal), 0, XMVectorGetZ(camHorizontal) });

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
				dx += static_cast<float>(rawDelta->x);
				dy += static_cast<float>(rawDelta->y);
			}
		}

		float mouseDx = static_cast<float>(dx) / static_cast<float>(winSize.x) * DirectX::XM_PI;
		float mouseDy = static_cast<float>(dy) / static_cast<float>(winSize.y) * DirectX::XM_PI;
		m_camera.rotate(mouseDx, mouseDy);

	}

};