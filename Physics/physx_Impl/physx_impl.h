#pragma once

#include "../PhysicEngine.h"
#include <PxPhysicsAPI.h>
#include <iostream>
using namespace physx;

struct Physx_Impl : public PhysicEngine::_ImplPhysic
{
private:
	// -- Define physx objects
	PxScene*				gScene			= NULL;
	PxDefaultAllocator		gAllocator;
	PxDefaultErrorCallback	gErrorCallback;
	PxFoundation*			gFoundation		= NULL;
	PxPhysics*				gPhysics		= NULL;
	PxDefaultCpuDispatcher* gDispatcher		= NULL;
	PxPvd*					gPvd			= NULL;
	PxMaterial*				gMaterial		= NULL;

	void createVehicule(const PxTransform& t, PxU32 size, PxReal halfExtent)
	{

		PxTransform vehiculeTransform(PxVec3(PxReal(0), PxReal(2 * halfExtent), PxReal(0)));
		PxBoxGeometry vehiculeGeometry(PxBoxGeometry(2 * halfExtent, 2 * halfExtent, 2 * halfExtent));
		PxRigidDynamic* vehicule = PxCreateDynamic(*gPhysics, t.transform(vehiculeTransform), vehiculeGeometry, *gMaterial, 10000.f);

		vehicule->setLinearVelocity(PxVec3(0));

		vehicule->setName("Vehicule");
		if (!gScene->addActor(*vehicule)) std::cout << "Echec de creation du vehicule" << std::endl;

	}
public:

	Physx_Impl() {};
	~Physx_Impl() { cleanupPhysics(false); };

private:

	virtual void onInit() override;

	virtual void onUpdate(float deltaTime) override {
		gScene->simulate(deltaTime);
		gScene->fetchResults(true);
	}

	virtual  void cleanupPhysics(bool /*interactive*/) override;

	/*virtual bool addActor(physx::PxActor* actor) override {
		return gScene->addActor(*actor);
	}*/



	// Getters
	PxScene* getScene() { return gScene; }
	PxPhysics* getPhysics() { return gPhysics; }
	PxMaterial* getMaterial() { return gMaterial; }
	PxDefaultCpuDispatcher* getDispatcher() { return gDispatcher; }
	PxDefaultAllocator* getAllocator() { return &gAllocator; }
	PxDefaultErrorCallback* getErrorCallback() { return &gErrorCallback; }
	PxFoundation* getFoundation() { return gFoundation; }
	PxPvd* getPvd() { return gPvd; }


};

