#pragma once

#include <utility>
#include <directXmath.h>
#include "../../Graphics/abstraction/Camera.h"
#include "World/Mesh.h"

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

	//Mesh m_mesh;




public:

	Cloporte();

	void handleInputs();
	void draw(Camera& cam);
	void update(float deltaTime);

	Camera& getCurrentCamera() noexcept { return (m_currentCamera == FIRST) ? m_cameras.first : m_cameras.second; }

private:
	void handleKeyboardInputs();

};

