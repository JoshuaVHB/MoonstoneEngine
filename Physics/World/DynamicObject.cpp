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

void DynamicObject::addShape(PxShape* shape)
{
	PxFilterData filterData;
	filterData.word0 = mFilterGroup;
	filterData.word1 = mMaskGroup;
	shape->setQueryFilterData(filterData);
	if(material) 
		shape->setMaterials(&material, 1);
	if (m_actor)
		m_actor->attachShape(*shape);
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
		PxRigidBodyExt::addForceAtPos(*m_actor->is<PxRigidDynamic>(), force, PxVec3(0.f, 10.f, 0.f));
		//m_actor->is<PxRigidDynamic>()->addForce(force/**0.15*/); //Facteur pour limiter la force pour arriver assez lentement a la maxLinearVelocity
	}
	//m_actor->is<PxRigidDynamic>()->
	
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

PhysicsEngine::fVec3 DynamicObject::getPosition()
{
	return m_actor->getGlobalPose().p;
}

void DynamicObject::setMass(float mass)
{
	if (m_actor && m_actor->is<PxRigidDynamic>()) {
		PxRigidBodyExt::setMassAndUpdateInertia(*m_actor->is<PxRigidDynamic>(), mass);
	}
}

void DynamicObject::setMaterial(float staticFriction, float restitution, float dynamicFriction)
{
	// Création d'un descripteur de matériau
	if(!material)
	{
		PxMaterial* material = PhysicsEngine::createMaterial(restitution, staticFriction, dynamicFriction);
		if (m_actor && m_actor->is<PxRigidDynamic>()) {
			const physx::PxU32 numShapes = m_actor->getNbShapes();
			physx::PxShape** shapes = new physx::PxShape * [numShapes];
			m_actor->getShapes(shapes, numShapes);

			for (physx::PxU32 i = 0; i < numShapes; i++) {
				shapes[i]->setMaterials(&material, 1);
			}

			delete[] shapes;
		}
	}
	else {
		material->setStaticFriction(staticFriction);
		material->setRestitution(restitution);
		material->setDynamicFriction(dynamicFriction);
	}

	

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


