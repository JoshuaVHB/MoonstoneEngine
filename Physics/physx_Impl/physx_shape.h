#pragma once
#include <PxPhysicsAPI.h>
#include "../PhysicEngine.h"
using namespace physx;
class physx_shape
{
public:
	static PxShape* getCube(const PhysicsEngine::fVec3 scale = PhysicsEngine::fVec3{ 1,1,1 });
	static PxShape* getBall(const float scale = 1.0f);
	static PxShape* getHeightmap(const PhysicsEngine::HeightMapData& data);
};

