#pragma once
#include "../../Graphics/World/Mesh.h"
#include <memory>
#include <string>

#include <PxPhysicsAPI.h>
#include "../PhysicEngine.h"
#include <DirectXMath.h>
class PhysicalObject
{
	static int count;
	std::unique_ptr<Mesh> m_mesh;
	std::string id;
	
public:
	PhysicalObject() : id{ std::string("PhysicalObject_") + std::to_string(++count) } {};
	~PhysicalObject() = default;

	Mesh* getMesh() { return m_mesh.get(); }
	void setMesh(Mesh* mesh) {

		m_mesh.reset(mesh); 
		Transform t = m_mesh->getTransform();
		auto pos_Render = t.getPosition();
		auto scale_Render = t.getScale();
		PhysicEngine::iVec3 pos{ DirectX::XMVectorGetX(pos_Render),  DirectX::XMVectorGetY(pos_Render),  DirectX::XMVectorGetZ(pos_Render) };
		PhysicEngine::iVec3 scale{ DirectX::XMVectorGetX(scale_Render),  DirectX::XMVectorGetY(scale_Render),  DirectX::XMVectorGetZ(scale_Render) };
		PhysicEngine::addCube(id, pos, PhysicEngine::iVec3(0,0,0), scale);
	
	}

	void updateTransform() { 
		PhysicEngine::iVec3 pos, rot;
			
		std::tie(pos, rot) = PhysicEngine::getTransform(id);
		Transform& t = m_mesh->getTransform();
		
		t.setTranslation(pos.x, pos.y, pos.z);
		t.setAngles(rot.x, rot.y, rot.z);

		//auto posT = t.getPosition();
		//std::cout << "pos Physic: " << DirectX::XMVectorGetX(posT) << " " << DirectX::XMVectorGetY(posT) << " " << DirectX::XMVectorGetZ(posT) << std::endl;
	}

	Transform& getTransform() { return m_mesh->getTransform(); }

	bool operator==(const PhysicalObject& other) const { return id == other.id; }


};

