#pragma once
#include "../../Graphics/World/Mesh.h"

class PhysicalObject
{
	std::unique_ptr<Mesh> m_mesh;
	
public:
	PhysicalObject(Mesh* mesh) : m_mesh{ mesh } {};
	~PhysicalObject() = default;


};

