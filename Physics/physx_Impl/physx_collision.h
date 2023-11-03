#pragma once

#include <PxPhysics.h>
#include <PxFiltering.h>
#include "../PhysicEngine.h"
using namespace physx;

class Physx_Collision : public PxSimulationEventCallback
{
	virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) override;

	// H�rit� via PxSimulationEventCallback
	void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) override;
	void onWake(PxActor** actors, PxU32 count) override;
	void onSleep(PxActor** actors, PxU32 count) override;
	void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override;
	void onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) override;
};

