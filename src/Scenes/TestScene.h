#pragma once

#include "Scene.h"

#include "../../Platform/WindowsEngine.h"
#include "../../Graphics/World/Cube.h"
#include "../../Graphics/abstraction/Camera.h"

#include "../../Platform/IO/Inputs.h"
#include <iostream>
#include <memory>

extern std::unique_ptr<Keyboard> wKbd;
extern std::unique_ptr<Mouse> wMouse;

using namespace MS;
class TestScene : public Scene {

private:

	Cube c;
	
	Camera camera;
	Vec delta = camera.getPosition();

	float dt = 0;
	float elapsed = 0;

	bool render = true;
	bool tmp = true;
	char text[500];


public:

	TestScene() {
		camera.setProjection<PerspectiveProjection>(PerspectiveProjection());	
		//WindowsEngine& rMoteur = WindowsEngine::getInstance();
	}


	virtual void onUpdate(float deltaTime) override
	{
		dt = deltaTime;
		elapsed += deltaTime;
		camera.setPosition(delta);
		camera.updateCam(deltaTime);

		Mouse::Event me = wMouse->read();
		if (me.leftIsPressed())
			std::cout << me.getPosX() << "|" << me.getPosY() << std::endl;

			/*
		glm::vec2 rotationMotion = Inputs::getMouseDelta() / Inputs::getInputRange() * Mathf::PI;

		m_camera.setYaw(m_camera.getYaw() - rotationMotion.x);
		m_camera.setPitch(std::max(-Mathf::PI * .499f, std::min(+Mathf::PI * .499f, m_camera.getPitch() + rotationMotion.y)));
			*/

		Keyboard::Event e = wKbd->readKey();
		if (e.isPress() && e.getCode()==VK_SPACE)
			render = !render;

		if (wKbd->isKeyPressed(Keyboard::letterCodeFromChar('q'))) {
			delta.vector4_f32[0] -= 0.5f;
		}

		if (wKbd->isKeyPressed(Keyboard::letterCodeFromChar('d'))) {
			delta.vector4_f32[0] += 0.5f;
		}

		if (wKbd->isKeyPressed(Keyboard::letterCodeFromChar('z'))) {
			delta.vector4_f32[2] += 0.5f;
		}

		if (wKbd->isKeyPressed(Keyboard::letterCodeFromChar('s'))) {
			delta.vector4_f32[2] -= 0.5f;
		}
	}

	virtual void onRender() override {
	
		Renderer::clearScreen(1.f,1.f,0.f,1.f);
		if (render) Renderer::renderMesh(camera, dt);
	}
	virtual void onImGuiRender()override {
		ImGui::Begin("Debug");

		ImGui::Text(std::to_string(render).c_str());
		ImGui::Text(std::to_string(wKbd->isKeyPressed(VK_SPACE)).c_str());
		ImGui::Text(std::to_string(tmp).c_str());
		ImGui::DragFloat4("delta", &delta.vector4_f32[0]);
		ImGui::InputText("coucou", text, 500);
		ImGui::End();

	}


};
