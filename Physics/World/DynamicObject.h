#pragma once
#include "PhysicalObject.h"
class DynamicObject : public PhysicalObject
{
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

	void setId(std::string id) { this->id = id; m_actor->setName(id.c_str()); }

	virtual void updateTransform() override;

	void addForce(PhysicsEngine::fVec3 force);
	void addTorque(PhysicsEngine::fVec3 Torque);
	void clearForce();
	void clearTorque();
	PhysicsEngine::fVec3 getLinearValocity();	
};

