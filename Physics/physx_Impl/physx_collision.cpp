#include "physx_collision.h"

void Physx_Collision::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
}

void Physx_Collision::onConstraintBreak(PxConstraintInfo* constraints, PxU32 count)
{
}

void Physx_Collision::onWake(PxActor** actors, PxU32 count)
{
}

void Physx_Collision::onSleep(PxActor** actors, PxU32 count)
{
}

void Physx_Collision::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
}

void Physx_Collision::onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count)
{
}
