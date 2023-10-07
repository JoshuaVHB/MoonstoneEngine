#pragma once

#include "Scene.h"

#include "../../Graphics/World/Cube.h"
#include "../../Graphics/abstraction/Camera.h"
#include "../../Graphics/abstraction/Shaders.h"
#include "../../Graphics/World/Mesh.h"
#include "../../Graphics/World/Cube.h"
#include "../../Graphics/World/Player.h"
#include "../../Graphics/World/WorldRendering/Skybox.h"

#include "../../Platform/IO/Inputs.h"
#include "../../Platform/WindowsEngine.h"
#include <iostream>
#include <memory>
#include <directXmath.h>




class TestScene : public Scene {

private:

	float dt = 0 , elapsed = 0;

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

	Player m_player;

public:

	TestScene() 
	{
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

		m_player.step(elapsed);
		worldParams sp;

		sp.viewProj = XMMatrixTranspose(m_player.getCamera().getVPMatrix());
		sp.lightPos = XMVectorSet(-10.0f * cos(elapsed), 10.0f * sin(elapsed), -10.0f, 1.0f);
		sp.cameraPos = m_player.getCamera().getPosition();
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

		Camera& cam = m_player.getCamera();
		Renderer::renderMesh(cam, terrainMesh, renderShader);

		//Renderer::renderMesh(camera, cube, renderShader);
		box.renderSkybox(cam);
		//Renderer::renderMesh(camera, box.getMesh(), renderShader);

	}
	virtual void onImGuiRender() override {
		ImGui::Begin("Debug");

		ImGui::Text(std::to_string(m_player.getCamera().getPosition().vector4_f32[0]).c_str());
		ImGui::End();

		m_player.onImGuiRender();

	}


};
