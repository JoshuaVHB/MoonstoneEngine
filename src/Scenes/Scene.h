#pragma once
#include <vector>

class Scene {

	virtual void onRender() = 0;
	virtual void onUpdate(float deltaTime) = 0;



};




class SceneManager
{

public:

	template<class _Scene>
		requires (std::derived_from<_Scene, Scene>)
	static void addScene() {
		m_scenes.push_back(new _Scene);
	}

	static void changeScene() {}

private:

	static std::vector<Scene*> m_scenes;



};