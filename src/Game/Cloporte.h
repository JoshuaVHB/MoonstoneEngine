#pragma once

#define _XM_NO_INTRINSICS_
#include <directXmath.h>
#include "../../Graphics/abstraction/Camera.h"
#include "World/Mesh.h"
#include "../../Physics/World/DynamicObject.h"

class Cloporte
{

private:


	DynamicObject m_object;
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
	DirectX::XMVECTOR m_groundDir	;
	


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

	
	const Mesh& getMesh()	{ 		
		return *(m_object.getMesh()); 
	}


	[[nodiscard]] const DynamicObject& getObject() const noexcept	{ return m_object; };
	[[nodiscard]] BoundingSphere getBoundingSphere() const noexcept { return m_boundingSphere; }
	[[nodiscard]] DirectX::XMVECTOR getForward() const noexcept		{ return m_forward; }
	[[nodiscard]] DirectX::XMVECTOR getPosition() const noexcept	{ return m_position; }
	[[nodiscard]] DirectX::XMVECTOR getGroundDir() const noexcept	{ return m_groundDir; }
	[[nodiscard]] float getMaxVelocity() const noexcept				{ return maxVelocity; }

	void setGroundVector(const DirectX::XMVECTOR& val)
	{
		m_groundDir = val;
	}

	void setPosition(float x, float y, float z) {
		m_object.clearForce();
		m_object.clearTorque();
		m_object.getTransform().setPosition({x,y,z});
		m_object.majTransformPhysics();
	}

	void setTranslation(float x, float y, float z)
	{
		m_object.clearForce();
		m_object.clearTorque();
		m_object.setTranslation(x, y, z);
		m_forward={ 0, 0, 1 , 0 };
		m_groundDir={ 0, 1, 0, 0 };
		currentVelocity={ 0 };
		m_object.setLinearVelocity({ 0,0,0 });
		m_object.setAngularVelocity({ 0,0,0 });
		
	}

private:
	void handleKeyboardInputs(float deltaTime);

};

