#pragma once

#include <concepts>
#include <memory>
#include <string>
#include <PxPhysicsAPI.h>
#include <vector>


#define PHYSIC_CALL_IMPL(fnName)\
static auto fnName(auto&& ...args)\
{\
	return implementation->##fnName(std::forward<decltype(args)>(args)...);\
}


class PhysicsEngine {
public:
	using fVec3 = physx::PxVec3;
	using Actor = physx::PxRigidActor;
	struct HeightMapData {
		unsigned int width, length;
		float scaleXZ, scaleY;
		std::vector<float> value;
		float maxHeight;
	};

	struct FilterGroup
	{
		enum Enum
		{
			ePlayer = (1 << 0),
			eCheckpoint = (1 << 1),
			eFinish = (1 << 2),
			eOther = (1 << 3),
			eAll = (1 << 4)
		};
	};


	struct _ImplPhysic
	{
		friend class PhysicsEngine;		

	protected:
		virtual void onInit() = 0;
		virtual void onUpdate(float deltaTime) = 0;
		virtual void cleanupPhysics(bool /*interactive*/) = 0;
		virtual std::pair<fVec3, fVec3> getTransform(std::string id) = 0;
		virtual bool addActor(Actor* actor) = 0;
		virtual Actor* createStaticActor(const fVec3 position = fVec3{0,0,0}) = 0;
		virtual Actor* createDynamicActor(const fVec3 position = fVec3{0,0,0}) = 0;
		
	};

	PHYSIC_CALL_IMPL(onInit);
	PHYSIC_CALL_IMPL(onUpdate);
	PHYSIC_CALL_IMPL(cleanupPhysics);
	PHYSIC_CALL_IMPL(getTransform);
	PHYSIC_CALL_IMPL(addActor);
	PHYSIC_CALL_IMPL(createStaticActor);
	PHYSIC_CALL_IMPL(createDynamicActor);


	template<typename _PhysicEngineImplementation>
		requires (std::derived_from<_PhysicEngineImplementation, _ImplPhysic>)
	static void setImplementation()
	{
		implementation = std::make_unique<_PhysicEngineImplementation>();
	}

private:

	static std::unique_ptr<_ImplPhysic> implementation;
};