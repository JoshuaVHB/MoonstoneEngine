#include "physx_impl.h"
#ifndef PHYSX_SNIPPET_PVD_H
#define PHYSX_SNIPPET_PVD_H
#include <iostream>
#include "../../Utils/Transform.h"
#define PVD_HOST "127.0.0.1"	//Set this to the IP address of the system running the PhysX Visual Debugger that you want to connect to.

#endif //PHYSX_SNIPPET_PVD_H

void Physx_Impl::onInit()
{
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;

	gScene = gPhysics->createScene(sceneDesc);

	PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	PxTransform transform(PxVec3(0, -10, 0));
	PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *gMaterial);
	groundPlane->setName("Ground");
	groundPlane->setGlobalPose(transform.transform(groundPlane->getGlobalPose()));
	gScene->addActor(*groundPlane);

	//createVehicule(PxTransform(PxVec3(0, 0, 10.0f)), 10, 2.0f);

}

void Physx_Impl::cleanupPhysics(bool)

{
	PX_RELEASE(gScene);
	PX_RELEASE(gDispatcher);
	PX_RELEASE(gPhysics);
	if (gPvd)
	{
		PxPvdTransport* transport = gPvd->getTransport();
		gPvd->release();
		gPvd = NULL;
		PX_RELEASE(transport);
	}
	PX_RELEASE(gFoundation);
}
#include <vector>
std::pair<PhysicEngine::iVec3, PhysicEngine::iVec3> Physx_Impl::getTransform(std::string id)
{
	const PxU32 nbActors = gScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
	std::vector<PxActor*> acteurs(nbActors);

	PxU32 nbActeursTrouves = gScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, acteurs.data(), nbActors);

	for (PxU32 i = 0; i < nbActeursTrouves; i++) {
		PxActor* acteur = acteurs[i];

		const char* nomActeurTrouve = acteur->getName();

		if (nomActeurTrouve && strcmp(nomActeurTrouve, id.c_str()) == 0) {
			
			if (acteur->is<PxRigidActor>())
			{
				PxRigidActor* actor = acteur->is<PxRigidActor>();
				PxTransform transform = actor->getGlobalPose();
				PxVec3 position = transform.p;
				PxVec3 rotation = transform.q.getBasisVector0();
				return std::make_pair<>(position, rotation);
			}
		}
	}
	std::cout << "Actor not found" << std::endl;
	return std::pair<PhysicEngine::iVec3, PhysicEngine::iVec3>{};

	
}

bool Physx_Impl::addCube(const std::string& id, const PhysicEngine::iVec3 position, const PhysicEngine::iVec3 rotation, const PhysicEngine::iVec3 scale)
{
	PxTransform transform(PxVec3(position.x, position.y, position.z), PxQuat(rotation.x, rotation.y, rotation.z, 1));
	PxBoxGeometry geometry(PxVec3(scale.x, scale.y, scale.z));
	PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, transform, geometry, *gMaterial, 10.0f);
	dynamic->setName(id.c_str());
	return gScene->addActor(*dynamic);
}
