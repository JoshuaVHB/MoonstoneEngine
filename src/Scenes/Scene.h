#pragma once


#include <vector>
#include <functional>
#include <string> 
#include <PxPhysicsAPI.h>
#include <iostream>
class Scene {
protected:

public:
	virtual ~Scene() = default;
	Scene() = default;

	virtual void onRender() = 0;
	virtual void onUpdate(float deltaTime) = 0;
	virtual void onImGuiRender() = 0;


};

using SceneBuildFn = std::function<Scene* ()>;
using SceneBuilder = std::pair<std::string, SceneBuildFn>;


class SceneManager
{

public:
	
	static void onUpdate(float deltaTime);
	static void onRender();
	static void onImGuiRender();

	static void registerScene(const std::string& name, const SceneBuildFn& provider);

	template<class _SceneInstance>
		requires (std::derived_from<_SceneInstance, Scene>)
	static void registerScene(const std::string& name) { registerScene(name, []() { return new _SceneInstance; }); }

	static void switchToScene(size_t index);

private:

	static std::vector<SceneBuilder> s_availableScenes;
	static size_t s_activeSceneIndex;
	static Scene* s_activeScene;


};