#pragma once

#include "Scene.h"

#include "../../Graphics/World/Cube.h"
#include "../../Graphics/abstraction/Camera.h"

class TestScene : public Scene {

private:

	Cube c;
	
	Camera camera;

	float dt = 0;
	float elapsed = 0;

public:

	TestScene() {
		camera.setProjection<PerspectiveProjection>(PerspectiveProjection());	
	}


	virtual void onUpdate(float deltaTime) override 
	{
		dt = deltaTime;
		elapsed+= deltaTime;
	}

	virtual void onRender() override {
	
		Renderer::clearScreen(1.f,1.f,0.f,1.f);
		Renderer::renderMesh(camera, dt);
	}
	virtual void onImGuiRender()override {}


};
