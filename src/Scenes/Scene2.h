#pragma once

#include "Scene.h"


class Scene2 : public Scene {
private:

	float r;

public:

	Scene2() {}


	virtual void onUpdate(float deltaTime) override {}

	virtual void onRender() override {
	
		Renderer::clearScreen(1, 0, 0, 1);
	
	}

	virtual void onImGuiRender() override {}


};
