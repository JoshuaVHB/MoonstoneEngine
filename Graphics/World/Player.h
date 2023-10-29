#pragma once


// This is the default FPS player.
// Windows specific

#define _XM_NO_INTRINSICS_
#include <DirectXMath.h>
#include "../abstraction/Camera.h"

class Player 
{
private:

	Camera				m_camera;
	DirectX::XMFLOAT3	m_position;
	DirectX::XMVECTOR	m_positionDelta{};
	bool				m_cursorIsConfined = false;
	bool				m_inputsAreLocked = false;

public:

	Player();

	Camera& getCamera() { return m_camera; }

	void step(float deltaTime);
	void onImGuiRender();
	void lockInputs(bool state);

private:

	void handleKeyboardEvents();
	void handleMouseEvents();

};