#pragma once
#include "PhysicalObject.h"
class DynamicObject : public PhysicalObject
{
	PxMaterial* material = nullptr;
public:
	DynamicObject() : PhysicalObject() { 
		m_actor = PhysicsEngine::createDynamicActor();
		m_actor->setName(id.c_str());
	};

	DynamicObject(PhysicsEngine::FilterGroup::Enum _filterGroup,
		PhysicsEngine::FilterGroup::Enum _maskGroup) : PhysicalObject(_filterGroup, _maskGroup) {
		m_actor = PhysicsEngine::createDynamicActor();
		m_actor->setName(id.c_str());
	};

	~DynamicObject() { if(material) material->release(); }


	void setId(std::string id) { this->id = id; m_actor->setName(id.c_str()); }

	virtual void updateTransform() override;
	void setTransform(Transform& _transform);
	void majTransformPhysics();
	virtual void addShape(PxShape* shape) override;

	void setMaxLinearVelocity(float maxLinearVelocity);
	void setMaxAngularVelocity(float maxAngularVelocity);

	void setLinearVelocity(PhysicsEngine::fVec3 linearVelocity);
	void setAngularVelocity(PhysicsEngine::fVec3 angularVelocity);

	void addForce(PhysicsEngine::fVec3 force);
	void addTorque(PhysicsEngine::fVec3 Torque);
	void clearForce();
	void clearTorque();

	PhysicsEngine::fVec3 getLinearValocity();	
	PhysicsEngine::fVec3 getForwardVector();
	PhysicsEngine::fVec3 getPosition();

	void setMass(float mass);
	void setMaterial(float restitution, float staticFriction,float dynamicFriction);

	void displayLinearVelocity();
	void displayAngularVelocity();

	void displayPosition();
};

