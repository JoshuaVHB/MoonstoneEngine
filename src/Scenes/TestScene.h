#pragma once

#include "Scene.h"


class TestScene : public Scene {

public:
	TestScene() {}


	virtual void onUpdate(float deltaTime) override {}
	virtual void onRender() override {
	
		Renderer::clearScreen(1.f,1.f,0.f,1.f);
	}
	virtual void onImGuiRender()override {}


};
