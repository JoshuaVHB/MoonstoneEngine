#pragma once

#include <utility>
#include <directXmath.h>
#include "../../Graphics/abstraction/Camera.h"

using Vec = DirectX::XMVECTOR;

class Cloporte
{

private:

	// 2 Cameras 
	// Mesh
	// Rigidbody
	// Inputs

	enum CameraMode {FIRST, THIRD};
	CameraMode m_currentCamera = THIRD;

	std::pair<Camera, Camera> m_cameras = std::make_pair(Camera{}, Camera{});

	Vec m_position;
	Vec m_forward;
	Vec m_accelation;
	float m_speed;



public:

	Cloporte();

	void handleInputs();
	void draw();
	void update(float deltaTime);


private:
	void handleKeyboardInputs();

};

