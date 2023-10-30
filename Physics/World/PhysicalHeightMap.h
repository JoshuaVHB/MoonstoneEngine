#pragma once
#ifndef LANDSCAPEPHYSICS
#define LANDSCAPEPHYSICS

#include "PhysicalObject.h"

class Terrain;

class PhysicalHeightMap : public PhysicalObject
{
private:
	std::unique_ptr<Terrain> terrain;
public :
	PhysicalHeightMap() : PhysicalObject() {};
	virtual void setMesh(Mesh* mesh) override { m_mesh.release(); };

	void setTerrain(Terrain* _terrain);


};


#endif // !LANDSCAPEPHYSICS