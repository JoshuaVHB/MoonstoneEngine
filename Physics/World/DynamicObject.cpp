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

void DynamicObject::setMaxLinearVelocity(float maxLinearVelocity)
{
	if (m_actor && m_actor->is<PxRigidDynamic>()) {
		m_actor->is<PxRigidDynamic>()->setMaxLinearVelocity(maxLinearVelocity);
	}
}

void DynamicObject::setMaxAngularVelocity(float maxAngularVelocity)
{
if (m_actor && m_actor->is<PxRigidDynamic>()) {
		m_actor->is<PxRigidDynamic>()->setMaxAngularVelocity(maxAngularVelocity);
	}
}

void DynamicObject::setLinearVelocity(PhysicsEngine::fVec3 linearVelocity)
{
	if (m_actor && m_actor->is<PxRigidDynamic>()) {
		m_actor->is<PxRigidDynamic>()->setLinearVelocity(linearVelocity);
	}
}

void DynamicObject::setAngularVelocity(PhysicsEngine::fVec3 angularVelocity)
{
	if (m_actor && m_actor->is<PxRigidDynamic>()) {
		m_actor->is<PxRigidDynamic>()->setAngularVelocity(angularVelocity);
	}
}

void DynamicObject::addForce(PhysicsEngine::fVec3 force)
{
	if (m_actor && m_actor->is<PxRigidDynamic>()) {
		m_actor->is<PxRigidDynamic>()->addForce(force/**0.15*/); //Facteur pour limiter la force pour arriver assez lentement a la maxLinearVelocity
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

void DynamicObject::displayLinearVelocity()
{
if (m_actor && m_actor->is<PxRigidDynamic>()) {
	auto v = m_actor->is<PxRigidDynamic>()->getLinearVelocity().magnitude();
	std::cout << "Linear Velocity : " << v << std::endl;
	}
}

void DynamicObject::displayAngularVelocity()
{
	if (m_actor && m_actor->is<PxRigidDynamic>()) {
		auto v = m_actor->is<PxRigidDynamic>()->getAngularVelocity().magnitude();
		std::cout << "Angular Velocity : " << v << std::endl;
	}
}

void DynamicObject::displayPosition()
{
	if (m_actor && m_actor->is<PxRigidDynamic>()) {
		auto v = m_actor->is<PxRigidDynamic>()->getGlobalPose().p;
		std::cout << "Position : " << v.x << " " << v.y << " " << v.z << std::endl;
	}
}


