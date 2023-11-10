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
public:
	using fVec3 = PhysicsEngine::fVec3;
	using group = PhysicsEngine::FilterGroup::Enum;
protected:
	static int count;
	std::string id;

protected:


	Mesh* m_mesh = nullptr;

	PhysicsEngine::Actor* m_actor = nullptr;
	group mFilterGroup
		= group::eOther;
	group mMaskGroup
		= group::eOther;

	PhysicalObject() : id{ std::string("PhysicalObject_") + std::to_string(++count) } {};
	PhysicalObject(group _filterGroup,	group _maskGroup)
		: id{ std::string("PhysicalObject_") + std::to_string(++count) }, 
			mFilterGroup{ _filterGroup }, mMaskGroup{ _maskGroup }
	{};
public:
	~PhysicalObject() = default;
	bool operator==(const PhysicalObject& other) const { return id == other.id; }

	Mesh* getMesh() { return m_mesh; }
	virtual void setMesh(Mesh* mesh);

	virtual void updateTransform() = 0;
	virtual Transform& getTransform() { static Transform t{};  return (m_mesh != nullptr) ? m_mesh->getTransform() : t; }
	
	std::string& getId() { return id; }

	virtual void addShape(PxShape* shape) {
		PxFilterData filterData;
		filterData.word0 = mFilterGroup;
		filterData.word1 = mMaskGroup;
		shape->setQueryFilterData(filterData);
		if(m_actor)
			m_actor->attachShape(*shape);
	}

};
#endif // !PHYSICALOBJECT_H
