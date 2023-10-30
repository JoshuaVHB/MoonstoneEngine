#pragma once

#define _XM_NO_INTRINSICS_
#include <directXmath.h>
#include "../../Graphics/abstraction/Camera.h"
#include "World/Mesh.h"


class Cloporte
{

private:


	Mesh m_mesh;
	BoundingSphere m_boundingSphere; // use this for collision check, and mesh aabb for culling

	Camera m_firstPerson;
	Camera m_thirdPerson;
	Camera* m_currentCam;

	float m_speed;
	float maxVelocity;
	float currentVelocity;
	float accelerationFactor;
	float friction;

	DirectX::XMVECTOR m_position	;
	DirectX::XMVECTOR m_forward		;

	


public:

	Cloporte();

	void handleInputs();
	void draw(Camera& cam);
	void update(float deltaTime);
	void updatePosition(float deltaTime);

	Camera& getCurrentCamera() noexcept { return *m_currentCam; }

	void switchView() noexcept
	{
		if (m_currentCam == &m_thirdPerson) m_currentCam = &m_firstPerson;
		else m_currentCam = &m_thirdPerson;
	}

	
	Mesh& getMesh()	{ return m_mesh; }


	[[nodiscard]] BoundingSphere getBoundingSphere() const noexcept { return m_boundingSphere; }
	[[nodiscard]] DirectX::XMVECTOR getForward() const noexcept { return m_forward; }
	[[nodiscard]] DirectX::XMVECTOR getPosition() const noexcept { return m_position; }

private:
	void handleKeyboardInputs(float deltaTime);

};

