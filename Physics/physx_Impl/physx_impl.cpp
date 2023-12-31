#include "physx_impl.h"
#ifndef PHYSX_SNIPPET_PVD_H
#define PHYSX_SNIPPET_PVD_H
#define PVD_HOST "127.0.0.1"	//Set this to the IP address of the system running the PhysX Visual Debugger that you want to connect to.
#endif //PHYSX_SNIPPET_PVD_H

#include <iostream>
#include <vector>
#include "../../Utils/Transform.h"
#include <PxPhysics.h>
#include <PxPhysicsAPI.h>
#include "physx_collision.h"
std::vector<Physx_Impl::ModulePhysics> Physx_Impl::physics = { Physx_Impl::ModulePhysics() };
int Physx_Impl::currentScene = 0;

Physx_Impl::ModulePhysics& Physx_Impl::getModulePhysics()
{
	return physics[currentScene];
}

bool Physx_Impl::changeScene(int numScene)
{
	if (numScene < physics.size())
	{
		currentScene = numScene;
		if (physics[currentScene].gScene == nullptr)
			onInit();
		return true;
	}
	return false;
}

int Physx_Impl::addScene()
{
	physics.push_back(ModulePhysics());
	return static_cast<int>(physics.size()) - 1;
}



PxFilterFlags CustomFilterShader(
	PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	// let triggers through
	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}

	// Check if it is a trigger interaction 
	if (filterData0.word0 & PhysicsEngine::FilterGroup::eFinish || filterData1.word0 & PhysicsEngine::FilterGroup::eFinish ||
		filterData0.word0 & PhysicsEngine::FilterGroup::eCheckpoint || filterData1.word0 & PhysicsEngine::FilterGroup::eCheckpoint)
	{
		if ((filterData0.word0 & filterData1.word1) && (filterData0.word1 & filterData1.word0))

			pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eSUPPRESS;
	}

	return PxFilterFlag::eDEFAULT;
}

void Physx_Impl::onInit()
{
	physics[currentScene].gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, physics[currentScene].gAllocator, physics[currentScene].gErrorCallback);

	physics[currentScene].gPvd = PxCreatePvd(*physics[currentScene].gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	physics[currentScene].gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	physics[currentScene].gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *physics[currentScene].gFoundation, PxTolerancesScale(), true, physics[currentScene].gPvd);

	PxSceneDesc sceneDesc(physics[currentScene].gPhysics->getTolerancesScale());

	sceneDesc.gravity = PxVec3(0.0f, -9.81f * 5.f, 0.0f);

	physics[currentScene].gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = physics[currentScene].gDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	//sceneDesc.filterShader = CustomFilterShader;
	//Physx_Collision* callback = new Physx_Collision();
	//sceneDesc.simulationEventCallback = callback;
	physics[currentScene].gScene = physics[currentScene].gPhysics->createScene(sceneDesc);

	physics[currentScene].gScene->setSimulationEventCallback(&CollisionCallback);


	PxPvdSceneClient* pvdClient = physics[currentScene].gScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	physics[currentScene].gMaterial = physics[currentScene].gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	//createVehicule(PxTransform(PxVec3(0, 0, 10.0f)), 10, 2.0f);

}

void Physx_Impl::cleanupPhysics(bool)

{
	PX_RELEASE(physics[currentScene].gScene);
	PX_RELEASE(physics[currentScene].gDispatcher);
	PX_RELEASE(physics[currentScene].gPhysics);
	if (physics[currentScene].gPvd)
	{
		PxPvdTransport* transport = physics[currentScene].gPvd->getTransport();
		physics[currentScene].gPvd->release();
		physics[currentScene].gPvd = NULL;
		PX_RELEASE(transport);
	}
	PX_RELEASE(physics[currentScene].gFoundation);
}

std::pair<PhysicsEngine::fVec3, PhysicsEngine::fVec3> Physx_Impl::getTransform(std::string id)
{
	const PxU32 nbActors = physics[currentScene].gScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
	std::vector<PxActor*> acteurs(nbActors);

	PxU32 nbActeursTrouves = physics[currentScene].gScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, acteurs.data(), nbActors);

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
	return std::pair<PhysicsEngine::fVec3, PhysicsEngine::fVec3>{};
}



