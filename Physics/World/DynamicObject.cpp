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

void DynamicObject::setTransform(Transform& _transform)
{
	m_mesh->getTransform() = _transform;
	majTransformPhysics();
}

void DynamicObject::majTransformPhysics()
{
	if (m_actor) {
		Transform t = m_mesh->getTransform();
		auto pos_Render = t.getTranslation();
		auto ang_Render = t.getAngles();
		PhysicsEngine::fVec3 position{ DirectX::XMVectorGetX(pos_Render),  DirectX::XMVectorGetY(pos_Render),  DirectX::XMVectorGetZ(pos_Render) };
		PhysicsEngine::fVec3 rotation{ DirectX::XMVectorGetX(ang_Render),  DirectX::XMVectorGetY(ang_Render),  DirectX::XMVectorGetZ(ang_Render) };
		m_actor->setGlobalPose(PxTransform(position, PxQuat(rotation.x, rotation.y, rotation.z, 1)));
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


