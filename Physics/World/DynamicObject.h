#pragma once
#include "PhysicalObject.h"
class DynamicObject : public PhysicalObject
{
public:
	DynamicObject() : PhysicalObject() { 
		m_actor = PhysicsEngine::createDynamicActor();
		m_actor->setName(id.c_str());
	};

	DynamicObject(group _filterGroup, group _maskGroup) 
		: PhysicalObject(_filterGroup, _maskGroup) {
		m_actor = PhysicsEngine::createDynamicActor();
		m_actor->setName(id.c_str());
	};




	void setId(std::string id) { this->id = id; m_actor->setName(id.c_str()); }

	virtual void updateTransform() override;
	void setTransform(Transform& _transform);
	void majTransformPhysics();
	virtual void addShape(PxShape* shape) override;

	void setMaxLinearVelocity(float maxLinearVelocity);
	void setMaxAngularVelocity(float maxAngularVelocity);

	void setLinearVelocity(fVec3 linearVelocity);
	void setAngularVelocity(fVec3 angularVelocity);

	void addForce(fVec3 force);
	void addTorque(fVec3 Torque);
	void clearForce();
	void clearTorque();

	fVec3 getLinearVelocity();	
	fVec3 getPosition();

	[[nodiscard]] float getLinearVelocityMag();

	void setMass(float mass);

	void displayLinearVelocity();
	void displayAngularVelocity();

	void displayPosition();
};

