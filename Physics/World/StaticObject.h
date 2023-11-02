#pragma once
#include "PhysicalObject.h"
class StaticObject : public PhysicalObject
{
public:
	StaticObject() : PhysicalObject() {
		m_actor = PhysicsEngine::createStaticActor();
		m_actor->setName(id.c_str());
	}

	virtual void updateTransform() override {}
};

