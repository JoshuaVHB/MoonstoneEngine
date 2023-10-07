#pragma once

#include "Scene.h"

#include "../../Graphics/World/Cube.h"
#include "../../Graphics/abstraction/Camera.h"
#include "../../Graphics/abstraction/Shaders.h"
#include "../../Graphics/World/Mesh.h"
#include "../../Graphics/World/Cube.h"
#include "../../Graphics/World/WorldRendering/Skybox.h"

#include "../../Platform/IO/Inputs.h"
#include "../../Platform/WindowsEngine.h"
#include <iostream>
#include <memory>
#include <directXmath.h>




inline std::ostream& operator<<(std::ostream& os, const Vec2& v) {
	os << "(" << (v.x) << "," << v.y << ")";
	return os;
}

class TestScene : public Scene {

private:

	
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
	Skybox box;

	struct worldParams {
		// la matrice totale 
		XMMATRIX viewProj;
		XMVECTOR lightPos; // la position de la source d’éclairage (Point)
		XMVECTOR cameraPos; // la position de la caméra 
		XMVECTOR ambiantLight; // la valeur ambiante de l’éclairage
		XMVECTOR diffuseLight; // la valeur ambiante du matériau 
		XMVECTOR ambiantMat; // la valeur diffuse de l’éclairage 
		XMVECTOR diffuseMat; // la valeur diffuse du matériau 
	};

	struct meshParams {

		XMMATRIX worldMat;
		XMMATRIX u_MVP;
	};



public:

	TestScene() 
	{
		camera.setProjection<PerspectiveProjection>(PerspectiveProjection());	
		winSize = WindowsEngine::getInstance().getGraphics().getWinSize();
		terrainMesh = Renderer::loadMeshFromFile("res/mesh/hello.obj");
		terrainMesh.m_worldMat = XMMatrixRotationX(3.141592f / 2.f);
		renderShader.loadEffectFromFile("res/effects/baseMesh.fx");
		cube = Cube::getCubeMesh();

		renderShader.addNewCBuffer("worldParams", sizeof(worldParams));
		renderShader.addNewCBuffer("meshParams", sizeof(meshParams));

		InputLayout testlayout;
		testlayout.pushBack<3>(InputLayout::Semantic::Position);
		testlayout.pushBack<3>(InputLayout::Semantic::Normal);
		testlayout.pushBack<2>(InputLayout::Semantic::Texcoord);
		auto tmplayout = testlayout.asInputDesc();

		renderShader.bindInputLayout(testlayout);

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

		worldParams sp;

		sp.viewProj = XMMatrixTranspose(camera.getVPMatrix());
		sp.lightPos = XMVectorSet(-10.0f * cos(elapsed), 10.0f * sin(elapsed), -10.0f, 1.0f);
		sp.cameraPos = camera.getPosition();
		sp.ambiantLight = XMVectorSet(.2f, 0.2f, 0.2f, 1.0f);
		sp.diffuseLight = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		sp.ambiantMat = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		sp.diffuseMat = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);

		renderShader.updateSubresource(sp, "worldParams");
		renderShader.sendCBufferToGPU("worldParams");

	}

	virtual void onRender() override {
	
		Renderer::clearScreen();
		renderShader.bindTexture("textureEntree", breadbug.getTexture());

		Renderer::renderMesh(camera, terrainMesh, renderShader);

		//Renderer::renderMesh(camera, cube, renderShader);

		box.renderSkybox(camera);

		//Renderer::renderMesh(camera, box.getMesh(), renderShader);

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
