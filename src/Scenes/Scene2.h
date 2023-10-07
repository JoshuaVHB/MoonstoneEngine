#pragma once

#include "Scene.h"

#include <filesystem>
#include "../../Graphics/World/WorldRendering/Terrain.h"

class Rush2Scene : public Scene {
private:


	//Mesh	m_terrainMesh;
	const std::filesystem::path path_to_map = "res/textures/heightmap.png";
	Terrain m_terrain{ path_to_map };

	Effect	m_baseMeshEffect;
	Skybox  m_skybox;

	Player  m_player; // Basically the 1st person camera
	Camera	m_thirdPerson;
	Camera* currentCamera = &m_thirdPerson;
	float	aspectRatio = 5;

	
	// Allows for hot reload of the heightmap
	std::filesystem::file_time_type ftime = std::filesystem::last_write_time(path_to_map);
	std::filesystem::file_time_type lastTime;

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

		// -- Setup the orthographic camera
		m_thirdPerson.setProjection<OrthographicProjection>(
			OrthographicProjection{1.f,50.f,50.f,1.f, 0.1f, 100.f}
		);
		m_thirdPerson.setPosition({ 0,30,15 });
		auto p = m_terrain.getParams();
		Vec centerPoint = { p.width / 2 * p.xyScale, 0.f, p.height / 2 * p.xyScale };
		m_thirdPerson.lookAt(centerPoint);


	}


	virtual void onUpdate(float deltaTime) override 
	{
		// Update the camera and handle inputs
		if (currentCamera != &m_thirdPerson) m_player.step(deltaTime);


		// -- Send world data to baseMesh effect
		worldParams sp;

		sp.viewProj		= XMMatrixTranspose(currentCamera->getVPMatrix());
		sp.lightPos		= XMVectorSet(-100.0f * cos(deltaTime), 100.f, -100.0f * sin(deltaTime), 1.0f); // spinning sun
		sp.cameraPos	= currentCamera->getPosition();
		sp.ambiantLight = XMVectorSet(.2f, 0.2f, 0.2f, 1.0f);
		sp.diffuseLight = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		sp.ambiantMat	= XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		sp.diffuseMat	= XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);

		m_baseMeshEffect.updateSubresource(sp, "worldParams");
		m_baseMeshEffect.sendCBufferToGPU("worldParams");

		// -- Check for hot reload
		lastTime = std::filesystem::last_write_time(path_to_map);
		if (lastTime != ftime) {
			m_terrain.hotreloadMap();
			ftime = lastTime;
		}



	}

	virtual void onRender() override {

		// Get our target camera
		Camera& cam = *currentCamera;

		// Clear and render objects
		Renderer::clearScreen();
		Renderer::renderMesh(cam, m_terrain.getMesh(), m_baseMeshEffect);

		// Render skybox last
		if (currentCamera != &m_thirdPerson) m_skybox.renderSkybox(cam);
	
	}

	virtual void onImGuiRender() override 
	{
	
		ImGui::Begin("Rush 2 debug window");
		if (ImGui::Button("Toggle Third person camera")) {

			if (currentCamera == &m_player.getCamera())
				currentCamera = &m_thirdPerson;
			else
				currentCamera = &m_player.getCamera();
		}

		if (ImGui::DragFloat("AspectRatio : ", &aspectRatio, 1.0f, 1.0f, 200) 	)
		{
			m_thirdPerson.setProjection<OrthographicProjection>(OrthographicProjection{ 1, aspectRatio, aspectRatio, 1, 0.1, 100 });
			auto p = m_terrain.getParams();
			Vec centerPoint = { p.width / 2 * p.xyScale, 0.f, p.height / 2 * p.xyScale };
			m_thirdPerson.lookAt(centerPoint);
		}

		ImGui::End();
		m_player.onImGuiRender();
		m_terrain.showDebugWindow();
	
	}


};
