#pragma once

#include <concepts>
#include <memory>
#include <string>
#include <PxPhysicsAPI.h>
#include <vector>
// Creates static function for the renderer 
// that calls the implementation function of the same name


#define PHYSIC_CALL_IMPL(fnName)\
static auto fnName(auto&& ...args)\
{\
	return implementation->##fnName(std::forward<decltype(args)>(args)...);\
}


class PhysicsEngine {
public:
	using iVec3 = physx::PxVec3;

	struct HeightMapData {
		unsigned int width, length;
		float scaleXZ, scaleY;
		std::vector<float> value;
		float maxHeight;
	};

	struct _ImplPhysic
	{
		friend class PhysicsEngine;		

	protected:
		virtual void onInit() = 0;
		virtual void onUpdate(float deltaTime) = 0;
		virtual void cleanupPhysics(bool /*interactive*/) = 0;
		virtual std::pair<iVec3, iVec3> getTransform(std::string id) = 0;
		virtual bool addCube(const std::string& id, const iVec3 position, const iVec3 rotation, const iVec3 scale = iVec3{1,1,1}) = 0;
		virtual bool addHeightmap(const std::string& id, const PhysicsEngine::HeightMapData &data) = 0;
		//virtual bool addBall(const std::string& id, const iVec3 position, const iVec3 rotation, const iVec3 scale = iVec3{ 1,1,1 }) = 0;
	};

	PHYSIC_CALL_IMPL(onInit);
	PHYSIC_CALL_IMPL(onUpdate);
	PHYSIC_CALL_IMPL(cleanupPhysics);
	PHYSIC_CALL_IMPL(getTransform);
	PHYSIC_CALL_IMPL(addCube);
	PHYSIC_CALL_IMPL(addHeightmap);



	template<typename _PhysicEngineImplementation>
		requires (std::derived_from<_PhysicEngineImplementation, _ImplPhysic>)
	static void setImplementation()
	{
		implementation = std::make_unique<_PhysicEngineImplementation>();
	}

private:

	static std::unique_ptr<_ImplPhysic> implementation;
};