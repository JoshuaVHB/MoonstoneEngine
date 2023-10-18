#pragma once

#include "Scene.h"
#include "Renderer.h"


class InGame : public Scene {
private:

	float r;

public:

	InGame() {}


	virtual void onUpdate(float deltaTime) override {}

	virtual void onRender() override {

		Renderer::clearScreen(0, 0, 0, 1);

	}

	virtual void onImGuiRender() override {}


};
