#pragma once

#include "Scene.h"


class Sponza : public Scene {
private:

	float r;

public:

	Sponza() {}


	virtual void onUpdate(float deltaTime) override {}

	virtual void onRender() override {

		Renderer::clearScreen(1, 0, 0, 1);

	}

	virtual void onImGuiRender() override {}


};
