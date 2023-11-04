#pragma once
#include "StaticObject.h"



class TriggerBox : public StaticObject
{
public:
	TriggerBox(PhysicsEngine::FilterGroup::Enum e) : StaticObject(e, 
		PhysicsEngine::FilterGroup::ePlayer) {
	};

	virtual void trigger() {

	}

	virtual void addShape(PxShape* shape) {
		PxFilterData filterData;
		filterData.word0 = mFilterGroup;
		filterData.word1 = mMaskGroup;
		shape->setQueryFilterData(filterData);
		shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

		
		if (m_actor)
			m_actor->attachShape(*shape);
		shape->release();
	}


};

