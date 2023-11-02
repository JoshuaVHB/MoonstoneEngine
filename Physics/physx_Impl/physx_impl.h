#pragma once

#include "../PhysicEngine.h"
#include <PxPhysicsAPI.h>
#include <iostream>
#include "physx_shape.h"
using namespace physx;

struct Physx_Impl : public PhysicsEngine::_ImplPhysic
{
public:
	friend class physx_shape;

	// -- Define physx objects
	struct ModulePhysics {
		PxScene* gScene = NULL;
		PxDefaultAllocator		gAllocator;
		PxDefaultErrorCallback	gErrorCallback;
		PxFoundation* gFoundation = NULL;
		PxPhysics* gPhysics = NULL;
		PxDefaultCpuDispatcher* gDispatcher = NULL;
		PxPvd* gPvd = NULL;
		PxMaterial* gMaterial = NULL;
	};
private:
	static ModulePhysics physics;

	static ModulePhysics& getModulePhysics();
private:
	// Getters
	PxScene* getScene() { return physics.gScene; }
	PxPhysics* getPhysics() { return physics.gPhysics; }
	PxMaterial* getMaterial() { return physics.gMaterial; }
	PxDefaultCpuDispatcher* getDispatcher() { return physics.gDispatcher; }
	PxDefaultAllocator* getAllocator() { return &physics.gAllocator; }
	PxDefaultErrorCallback* getErrorCallback() { return &physics.gErrorCallback; }
	PxFoundation* getFoundation() { return physics.gFoundation; }
	PxPvd* getPvd() { return physics.gPvd; }

public:
	Physx_Impl() {};
	~Physx_Impl() { cleanupPhysics(false); };

private:

	virtual void onInit() override;

	virtual void onUpdate(float deltaTime) override {
		physics.gScene->simulate(deltaTime);
		physics.gScene->fetchResults(true);
	}

	virtual  void cleanupPhysics(bool /*interactive*/) override;

	/*virtual bool addActor(physx::PxActor* actor) override {
		return gScene->addActor(*actor);
	}*/

	virtual std::pair<PhysicsEngine::fVec3, PhysicsEngine::fVec3> getTransform(std::string id);

	virtual bool addActor(PhysicsEngine::Actor* actor) override {
		return physics.gScene->addActor(*actor);
	}

	virtual PhysicsEngine::Actor* createStaticActor(
		const PhysicsEngine::fVec3 position = PhysicsEngine::fVec3{ 0,0,0 }) override {
		PxRigidActor * a = physics.gPhysics->createRigidStatic(PxTransform(position));
		addActor(a);
		return a;
	}		
	virtual PhysicsEngine::Actor* createDynamicActor(
		const PhysicsEngine::fVec3 position = PhysicsEngine::fVec3{ 0,0,0 }) override {
		PxRigidActor* a = physics.gPhysics->createRigidDynamic(PxTransform(position));
		addActor(a);
		return a;
	}




};

