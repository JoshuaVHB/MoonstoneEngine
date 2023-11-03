#pragma once
#include "PhysicalObject.h"
class StaticObject : public PhysicalObject
{
public:
	StaticObject() : PhysicalObject() {
		m_actor = PhysicsEngine::createStaticActor();
		m_actor->setName(id.c_str());
	}

	StaticObject(PhysicsEngine::FilterGroup::Enum _filterGroup, 
		PhysicsEngine::FilterGroup::Enum _maskGroup) : PhysicalObject(_filterGroup, _maskGroup) {
		m_actor = PhysicsEngine::createDynamicActor();
		m_actor->setName(id.c_str());
	}

	virtual void updateTransform() override {}
};

