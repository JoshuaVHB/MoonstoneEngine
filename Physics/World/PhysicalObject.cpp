#include "PhysicalObject.h"

int PhysicalObject::count = 0;

void PhysicalObject::setMesh(Mesh* mesh)
{
	//m_mesh.reset(mesh);
	m_mesh = mesh;
	if (m_actor) {
		Transform t = m_mesh->getTransform();
		auto pos_Render = t.getTranslation();
		auto ang_Render = t.getAngles();
		PhysicsEngine::fVec3 position{ DirectX::XMVectorGetX(pos_Render),  DirectX::XMVectorGetY(pos_Render),  DirectX::XMVectorGetZ(pos_Render) };
		PhysicsEngine::fVec3 rotation{ DirectX::XMVectorGetX(ang_Render),  DirectX::XMVectorGetY(ang_Render),  DirectX::XMVectorGetZ(ang_Render) };
		m_actor->setGlobalPose(PxTransform(position, PxQuat(rotation.x, rotation.y, rotation.z, 1)));
	}
}