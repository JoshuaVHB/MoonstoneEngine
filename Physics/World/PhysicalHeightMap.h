#pragma once
#ifndef LANDSCAPEPHYSICS
#define LANDSCAPEPHYSICS

#include "StaticObject.h"

class Terrain;

class PhysicalHeightMap : public StaticObject
{
private:
	std::unique_ptr<Terrain> terrain;
public :
	PhysicalHeightMap() : StaticObject() {};
	virtual void setMesh(Mesh*) override {};

	void setTerrain(Terrain* _terrain);
};


#endif // !LANDSCAPEPHYSICS