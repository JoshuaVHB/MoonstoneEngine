#include "physx_collision.h"
#include "../World/TriggerBox.h"
#include <iostream>
void Physx_Collision::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
    for (PxU32 i = 0; i < count; i++) {
        // V�rifiez si la trigger box est impliqu�e dans la paire
        if (pairs[i].triggerActor->is<PxRigidActor>()) {
            // R�cup�ration des formes de l'acteur
            try {
                reinterpret_cast<TriggerBox*>(pairs[i].triggerActor->userData)->onTrigger();
            }
            catch (...) {std::cout << "Error onTrigger" << std::endl;
			}
        }
    }
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
