#include "DynamicObject.h"

void DynamicObject::updateTransform()
{
	if (m_actor) {
		PxTransform transform = m_actor->getGlobalPose();
		PxVec3 position = transform.p;
		PxQuat rotation = transform.q;
		Transform& t = m_mesh->getTransform();

		t.setTranslation(position.x, position.y, position.z);
		t.setAngles(rotation.x, rotation.y, rotation.z);
	}
}

void DynamicObject::addForce(PhysicsEngine::fVec3 force)
{
	if (m_actor && m_actor->is<PxRigidDynamic>()) {
		m_actor->is<PxRigidDynamic>()->addForce(force);
	}
}

void DynamicObject::addTorque(PhysicsEngine::fVec3 torque)
{
	if (m_actor && m_actor->is<PxRigidDynamic>()) {
		m_actor->is<PxRigidDynamic>()->addTorque(torque);
	}
}

void DynamicObject::clearForce()
{
	if (m_actor && m_actor->is<PxRigidDynamic>()) {
		m_actor->is<PxRigidDynamic>()->clearForce();
	}
}

void DynamicObject::clearTorque()
{
	if (m_actor && m_actor->is<PxRigidDynamic>()) {
		m_actor->is<PxRigidDynamic>()->clearTorque();
	}
}

PhysicsEngine::fVec3 DynamicObject::getLinearValocity()
{
	if (m_actor && m_actor->is<PxRigidDynamic>()) {
		return m_actor->is<PxRigidDynamic>()->getLinearVelocity();
	}
	return PhysicsEngine::fVec3{};
}
