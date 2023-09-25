#pragma once

#include "Scene.h"

#include "../../Graphics/World/Cube.h"
#include "../../Graphics/abstraction/Camera.h"

class TestScene : public Scene {

private:

	Cube c;
	
	Camera camera;

public:

	TestScene() {
		camera.setProjection<OrthographicProjection>({0,1,0,1,.1f, 10.f});
	
	}


	virtual void onUpdate(float deltaTime) override {}
	virtual void onRender() override {
	
		//Renderer::renderMesh();
		Renderer::clearScreen(1.f,1.f,0.f,1.f);
	}
	virtual void onImGuiRender()override {}


};
