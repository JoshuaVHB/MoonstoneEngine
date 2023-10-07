#pragma once

#include "Scene.h"


class Rush2Scene : public Scene {
private:


	Mesh	m_terrainMesh;
	Effect	m_baseMeshEffect;
	Skybox  m_skybox;
	Player  m_player; // Basically the camera

	Texture breadbug = Texture(L"res/textures/breadbug.dds");

private:

	struct worldParams {
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
	};

public:

	Rush2Scene() 
	{
		// -- Load .obj file to terrainMesh
		m_terrainMesh = Renderer::loadMeshFromFile("res/mesh/hello.obj");

		// -- Rotate horizontally (due to different coordinate systems)
		m_terrainMesh.m_worldMat = XMMatrixRotationX(DirectX::XM_PI / 2.f);

		// -- Import the baseMesh effect
		m_baseMeshEffect.loadEffectFromFile("res/effects/baseMesh.fx");

		// -- Setup the effect correctly

		m_baseMeshEffect.addNewCBuffer("worldParams", sizeof(worldParams)); // For camera, light, colors...
		m_baseMeshEffect.addNewCBuffer("meshParams", sizeof(meshParams)); // For model matrix basically

		// -- Specify how the input vertices are layered
		InputLayout testlayout;
		testlayout.pushBack<3>(InputLayout::Semantic::Position);
		testlayout.pushBack<3>(InputLayout::Semantic::Normal);
		testlayout.pushBack<2>(InputLayout::Semantic::Texcoord);

		m_baseMeshEffect.bindInputLayout(testlayout);


	}


	virtual void onUpdate(float deltaTime) override 
	{
		// Update the camera and handle inputs
		m_player.step(deltaTime); 


		// -- Send world data to baseMesh effect
		worldParams sp;

		sp.viewProj		= XMMatrixTranspose(m_player.getCamera().getVPMatrix());
		sp.lightPos		= XMVectorSet(-10.0f * cos(deltaTime), 10.0f * sin(deltaTime), -10.0f, 1.0f); // spinning sun
		sp.cameraPos	= m_player.getCamera().getPosition();
		sp.ambiantLight = XMVectorSet(.2f, 0.2f, 0.2f, 1.0f);
		sp.diffuseLight = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		sp.ambiantMat	= XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		sp.diffuseMat	= XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);

		m_baseMeshEffect.updateSubresource(sp, "worldParams");
		m_baseMeshEffect.sendCBufferToGPU("worldParams");


	}

	virtual void onRender() override {

		// Get our target camera
		Camera& cam = m_player.getCamera();

		// Clear and render objects
		Renderer::clearScreen();
		m_baseMeshEffect.bindTexture("textureEntree", breadbug.getTexture());
		Renderer::renderMesh(cam, m_terrainMesh, m_baseMeshEffect);

		// Render skybox last
		m_skybox.renderSkybox(cam);	
	
	}

	virtual void onImGuiRender() override 
	{
	
		m_player.onImGuiRender();
	
	}


};
