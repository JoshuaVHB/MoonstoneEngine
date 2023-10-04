#pragma once

#include "Scene.h"

#include "../../Graphics/World/Cube.h"
#include "../../Graphics/abstraction/Camera.h"
#include "../../Graphics/abstraction/Shaders.h"
#include "../../Graphics/World/Mesh.h"
#include "../../Graphics/World/Cube.h"

#include "../../Platform/IO/Inputs.h"
#include "../../Platform/WindowsEngine.h"
#include <iostream>
#include <memory>
#include <directXmath.h>


extern std::unique_ptr<Keyboard> wKbd;
extern std::unique_ptr<Mouse> wMouse;



inline std::ostream& operator<<(std::ostream& os, const Vec2& v) {
	os << "(" << (v.x) << "," << v.y << ")";
	return os;
}

class TestScene : public Scene {

private:

	Cube c;
	
	Camera camera;
	Vec delta = camera.getPosition();

	float dt = 0;
	float elapsed = 0;

	bool tmp = true;
	char text[500] = {};

	Vec2 mousePose;
	Vec2 mousePreviousPose;
	Vec2 mouseDelta;

	std::pair<int, int> winSize{};

	Mesh terrainMesh, cube;
	Effect renderShader;
	Texture breadbug = Texture(L"res/textures/breadbug.dds");

public:

	TestScene() 
	{
		camera.setProjection<PerspectiveProjection>(PerspectiveProjection());	
		winSize = WindowsEngine::getInstance().getGraphics().getWinSize();
		terrainMesh = Renderer::loadMeshFromFile("res/mesh/mesh.obj");
		renderShader.loadEffectFromFile("res/effects/MiniPhong.fx");
		cube = Cube::getCubeMesh();
	}


	virtual void onUpdate(float deltaTime) override
	{
		dt = deltaTime;
		elapsed += deltaTime;

		Mouse::Event me;

		while ((me = wMouse->read()).isValid()) {

			mouseDelta = Vec2(me.getPos()) - mousePreviousPose;
			mousePreviousPose = Vec2(me.getPos());

			Vec2 rotationMotion = (mouseDelta / Vec2(winSize)) * 3.1415926f;
			//std::cout << mousePose << "|" << rotationMotion << std::endl;
			float yaw = camera.getYaw() + rotationMotion.x;

			float pitch = max(-3.1415926f * .499f,
				min(3.1415926f * .499f,
					camera.getPitch() + rotationMotion.y
				)
			);

			camera.setYaw(yaw);
			camera.setPitch(pitch);
		}
		Vec camForward = camera.getForwardDir();
		Vec camHorizontal = camera.getHorizontalDir();

		Keyboard::Event e = wKbd->readKey();

		if (wKbd->isKeyPressed(VK_SPACE)) {
			delta = XMVectorSetY(delta, XMVectorGetY(delta) + 0.5f);
		}
		else if (wKbd->isKeyPressed(VK_SHIFT)) {
			delta = XMVectorSetY(delta, XMVectorGetY(delta) - 0.5f);
		}

		if (wKbd->isKeyPressed(Keyboard::letterCodeFromChar('q'))) {
			delta = XMVectorSetZ(delta, XMVectorGetZ(delta) - XMVectorGetZ(camHorizontal));
			delta = XMVectorSetX(delta, XMVectorGetX(delta) - XMVectorGetX(camHorizontal));
		} 

		else if (wKbd->isKeyPressed(Keyboard::letterCodeFromChar('d'))) {
			delta = XMVectorSetZ(delta, XMVectorGetZ(delta) + XMVectorGetZ(camHorizontal));
			delta = XMVectorSetX(delta, XMVectorGetX(delta) + XMVectorGetX(camHorizontal));
		}

		if (wKbd->isKeyPressed(Keyboard::letterCodeFromChar('z'))) {
			delta = XMVectorSetZ(delta, XMVectorGetZ(delta) - XMVectorGetZ(camForward));
			delta = XMVectorSetX(delta, XMVectorGetX(delta) - XMVectorGetX(camForward));
		}

		else if (wKbd->isKeyPressed(Keyboard::letterCodeFromChar('s'))) {
			delta = XMVectorSetZ(delta, XMVectorGetZ(delta) + XMVectorGetZ(camForward));
			delta = XMVectorSetX(delta, XMVectorGetX(delta) + XMVectorGetX(camForward));
		}


		camera.setPosition(delta);
		camera.updateCam(deltaTime);
	}

	virtual void onRender() override {
	
		Renderer::clearScreen();
		renderShader.bindTexture("textureEntree", breadbug.getTexture());
		Renderer::renderMesh(camera, terrainMesh, renderShader);
		Renderer::renderMesh(camera, cube, renderShader);

	}
	virtual void onImGuiRender() override {
		ImGui::Begin("Debug");

		ImGui::Text(std::to_string(wKbd->isKeyPressed(VK_SPACE)).c_str());
		ImGui::Text(std::to_string(tmp).c_str());
		ImGui::DragFloat4("delta", &delta.vector4_f32[0]);
		ImGui::InputText("coucou", text, 500);
		ImGui::End();

	}


};
