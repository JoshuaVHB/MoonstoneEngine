#pragma once
#ifndef PHYSICALOBJECT_H
#define PHYSICALOBJECT_H
#include "../../Graphics/World/Mesh.h"
#include <memory>
#include <string>
#include "../physx_Impl/physx_shape.h"
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
	PhysicsEngine::Actor* m_actor = nullptr;
	PhysicalObject() : id{ std::string("PhysicalObject_") + std::to_string(++count) } {};
public:
	~PhysicalObject() = default;
	bool operator==(const PhysicalObject& other) const { return id == other.id; }

	Mesh* getMesh() { return m_mesh.get(); }
	virtual void setMesh(Mesh* mesh);

	virtual void updateTransform() = 0;
	Transform& getTransform() { return m_mesh->getTransform(); }	
	
	std::string& getId() { return id; }

	virtual void addShape(PxShape* shape) {
		if(m_actor)
			m_actor->attachShape(*shape);
	}
};
#endif // !PHYSICALOBJECT_H
