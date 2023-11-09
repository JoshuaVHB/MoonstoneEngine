#pragma once

#include "../PhysicEngine.h"
#include <PxPhysicsAPI.h>
#include <iostream>
#include "physx_shape.h"
#include "physx_collision.h"
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
	static std::vector<ModulePhysics> physics;
	static int currentScene;
	static ModulePhysics& getModulePhysics();

	Physx_Collision CollisionCallback{};
private:
	virtual bool changeScene(int numScene) override;
	virtual int addScene() override;

private:
	// Getters
	PxScene* getScene() { return physics[currentScene].gScene; }
	PxPhysics* getPhysics() { return physics[currentScene].gPhysics; }
	PxMaterial* getMaterial() { return physics[currentScene].gMaterial; }
	PxDefaultCpuDispatcher* getDispatcher() { return physics[currentScene].gDispatcher; }
	PxDefaultAllocator* getAllocator() { return &physics[currentScene].gAllocator; }
	PxDefaultErrorCallback* getErrorCallback() { return &physics[currentScene].gErrorCallback; }
	PxFoundation* getFoundation() { return physics[currentScene].gFoundation; }
	PxPvd* getPvd() { return physics[currentScene].gPvd; }

public:
	Physx_Impl() {};
	~Physx_Impl() { cleanupPhysics(false); };

private:

	virtual void onInit() override;

	virtual void onUpdate(float deltaTime) override {
		physics[currentScene].gScene->simulate(deltaTime);
		physics[currentScene].gScene->fetchResults(true);
	}

	virtual  void cleanupPhysics(bool /*interactive*/) override;

	/*virtual bool addActor(physx::PxActor* actor) override {
		return gScene->addActor(*actor);
	}*/

	virtual std::pair<PhysicsEngine::fVec3, PhysicsEngine::fVec3> getTransform(std::string id);

	virtual bool addActor(PhysicsEngine::Actor* actor) override {
		return physics[currentScene].gScene->addActor(*actor);
	}

	virtual PhysicsEngine::Actor* createStaticActor(
		const PhysicsEngine::fVec3 position = PhysicsEngine::fVec3{ 0,0,0 }) override {
		PxRigidActor * a = physics[currentScene].gPhysics->createRigidStatic(PxTransform(position));
		addActor(a);
		return a;
	}		
	virtual PhysicsEngine::Actor* createDynamicActor(
		const PhysicsEngine::fVec3 position = PhysicsEngine::fVec3{ 0,0,0 }) override {
		PxRigidActor* a = physics[currentScene].gPhysics->createRigidDynamic(PxTransform(position));
		addActor(a);
		return a;
	}

	virtual PxMaterial* createMaterial(float restitution = 0.f, float staticFriction = 0.f, float dynamicFriction = 0.f) {
		return physics[currentScene].gPhysics->createMaterial(staticFriction, dynamicFriction, restitution);
	}


};

