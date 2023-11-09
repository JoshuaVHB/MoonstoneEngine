#include "TriggerBox.h"

TriggerBox::TriggerBox(fVec3 position, fVec3 scale) : StaticObject()
{
	setPosition(position);
	addShape(physx_shape::getCube(scale));
	m_actor->userData = this;
}

TriggerBox::TriggerBox(group e, fVec3 position, fVec3 scale)
	: StaticObject(e, group::ePlayer)
{
	setPosition(position);
	addShape(physx_shape::getCube(scale));
	m_actor->userData = this;
}

void TriggerBox::addShape(PxShape* shape)
{	
	PxFilterData filterData;
	filterData.word0 = mFilterGroup;
	filterData.word1 = mMaskGroup;
	
	shape->setQueryFilterData(filterData);
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	if (m_actor)
		m_actor->attachShape(*shape);
	shape->release();
}

void TriggerBox::setPosition(fVec3 pos)
{
	PxTransform transform = m_actor->getGlobalPose();
	transform.p = pos;
	m_actor->setGlobalPose(transform);
}

void TriggerBox::setTriggerCallback(std::function<void()>&& callback)
{
	m_triggerCallback = callback;
}

void TriggerBox::onTrigger()
{
	m_triggerCallback();
}
