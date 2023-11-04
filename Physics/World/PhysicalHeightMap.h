#pragma once
#ifndef LANDSCAPEPHYSICS
#define LANDSCAPEPHYSICS

#include "StaticObject.h"

class Terrain;

class PhysicalHeightMap : public StaticObject
{
private:
	const Terrain* terrain = nullptr; // don't hold a unique pointer that steal ownership
public :
	PhysicalHeightMap() : StaticObject() {};
	virtual void setMesh(Mesh*) override {};

	void setTerrain(const Terrain* _terrain);
};


#endif // !LANDSCAPEPHYSICS