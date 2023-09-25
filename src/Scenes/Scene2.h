#pragma once

#include "Scene.h"


class Scene2 : public Scene {
private:

	float r;

public:
	Scene2() {}


	virtual void onUpdate(float deltaTime) override {}
	virtual void onRender() override {

		Renderer::clearScreen(r, 1.f, 1.f, 1.f);
	}
	virtual void onImGuiRender() override {
	
		ImGui::Begin("Bonjour");
		ImGui::DragFloat("red", &r, 0.1f, 0, 1.f);
		ImGui::End();
	
	
	}


};
