#pragma once

#include "Scene.h"


class Scene2 : public Scene {

public:
	Scene2() {}


	virtual void onUpdate(float deltaTime) override {}
	virtual void onRender() override {

		Renderer::clearScreen(0.f, 1.f, 1.f, 1.f);
	}
	virtual void onImGuiRender() override {
	
	
	
	}


};
