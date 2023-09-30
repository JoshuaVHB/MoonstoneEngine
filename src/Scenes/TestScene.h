#pragma once

#include "Scene.h"

#include "../../Platform/WindowsEngine.h"
#include "../../Graphics/World/Cube.h"
#include "../../Graphics/abstraction/Camera.h"

#include "../../Platform/IO/Inputs.h"
#include <memory>

extern std::unique_ptr<Keyboard> wKbd;

using namespace MS;
class TestScene : public Scene {

private:

	Cube c;
	
	Camera camera;
	Vec delta = camera.getPosition();

	float dt = 0;
	float elapsed = 0;

	bool render = true;

public:

	TestScene() {
		camera.setProjection<PerspectiveProjection>(PerspectiveProjection());	
		//WindowsEngine& rMoteur = WindowsEngine::getInstance();
		bool a = wKbd->isKeyPressed(VK_SPACE);
	}


	virtual void onUpdate(float deltaTime) override
	{
		dt = deltaTime;
		elapsed += deltaTime;
		camera.setPosition(delta);
		camera.updateCam(deltaTime);
		if (wKbd->readKey(VK_SPACE).isPress()) render = !render;
	}

	virtual void onRender() override {
	
		Renderer::clearScreen(1.f,1.f,0.f,1.f);
		if (render) Renderer::renderMesh(camera, dt);
	}
	virtual void onImGuiRender()override {
		ImGui::Begin("Debug");

		ImGui::Text(std::to_string(render).c_str());
		ImGui::Text(std::to_string(wKbd->isKeyPressed(VK_SPACE)).c_str());
		ImGui::DragFloat4("delta", &delta.vector4_f32[0]);
		ImGui::End();

	}


};
