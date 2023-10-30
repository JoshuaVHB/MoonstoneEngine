#pragma once
#ifndef PHYSICALOBJECT_H
#define PHYSICALOBJECT_H
#include "../../Graphics/World/Mesh.h"
#include <memory>
#include <string>

#include <PxPhysicsAPI.h>
#include "../PhysicEngine.h"
#include <DirectXMath.h>
class PhysicalObject
{
protected:
	static int count;
	std::string id;

protected:
	std::unique_ptr<Mesh> m_mesh;
	
public:
	PhysicalObject() : id{ std::string("PhysicalObject_") + std::to_string(++count) } {};
	~PhysicalObject() = default;

	Mesh* getMesh() { return m_mesh.get(); }
	virtual void setMesh(Mesh* mesh) {

		m_mesh.reset(mesh); 
		Transform t = m_mesh->getTransform();
		auto pos_Render = t.getPosition();
		auto scale_Render = t.getScale();
		PhysicsEngine::iVec3 pos{ DirectX::XMVectorGetX(pos_Render),  DirectX::XMVectorGetY(pos_Render),  DirectX::XMVectorGetZ(pos_Render) };
		PhysicsEngine::iVec3 scale{ DirectX::XMVectorGetX(scale_Render),  DirectX::XMVectorGetY(scale_Render),  DirectX::XMVectorGetZ(scale_Render) };
		PhysicsEngine::addCube(id, pos, PhysicsEngine::iVec3(0,0,0), scale);
	
	}

	void updateTransform() { 
		PhysicsEngine::iVec3 pos, rot;
			
		std::tie(pos, rot) = PhysicsEngine::getTransform(id);
		Transform& t = m_mesh->getTransform();
		
		t.setTranslation(pos.x, pos.y, pos.z);
		t.setAngles(rot.x, rot.y, rot.z);
	}

	Transform& getTransform() { return m_mesh->getTransform(); }

	bool operator==(const PhysicalObject& other) const { return id == other.id; }


};
#endif // !PHYSICALOBJECT_H
