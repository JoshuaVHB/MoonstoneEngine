#pragma once

#include "Scene.h"

#include <filesystem>
#include "../../Graphics/World/WorldRendering/Terrain.h"

class Rush2Scene : public Scene {
private:


	//Mesh	m_terrainMesh;
	const std::filesystem::path path_to_map = "res/textures/full.png";
	Terrain m_terrain{ path_to_map };
	Texture m_rockTexture{ L"res/textures/rock.dds" };
	Texture m_grassTexture{ L"res/textures/grass6.dds" };

	Effect	m_baseMeshEffect;
	Skybox  m_skybox;

	Player  m_player; // Basically the 1st person camera
	Camera	m_orthoCam;
	Camera* currentCamera = &m_orthoCam;
	Vec m_orthocamPos = { 0,120,50 }; // For imgui purposes

	float	aspectRatio = 100;

	float m_elapsedTime = 0;
	
	// Allows for hot reload of the heightmap
	std::filesystem::file_time_type ftime = std::filesystem::last_write_time(path_to_map);
	std::filesystem::file_time_type lastTime;

	Vec centerPoint;

private:

	struct worldParams {
		XMMATRIX viewProj;
		XMVECTOR lightPos; // la position de la source d’éclairage (Point)
		XMVECTOR cameraPos; // la position de la caméra 
		XMVECTOR sunColor = {1,1,.8,1}; // la valeur ambiante de l’éclairage
		XMVECTOR sunStrength = { 0.75 }; // la valeur ambiante du matériau 
	} sp ;

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
		m_orthoCam.setProjection<OrthographicProjection>(
			OrthographicProjection{-5.f,5,5,-5, 0, 10000.f}
		);
		m_orthoCam.setPosition(m_orthocamPos);


		Terrain::TerrainParams p = m_terrain.getParams();
		centerPoint = { p.width / 2 * p.xyScale, 0.f, p.height / 2 * p.xyScale };
	}


	virtual void onUpdate(float deltaTime) override 
	{
		m_elapsedTime += deltaTime;

		// Update cameras and handle inputs
		if (currentCamera != &m_orthoCam) m_player.step(deltaTime);
		else {
			m_orthoCam.updateCam();
			m_orthoCam.lookAt(centerPoint);
		}
		// -- Send world data to baseMesh effect
		sp.viewProj		= XMMatrixTranspose(currentCamera->getVPMatrix());
		sp.lightPos		= XMVectorSet(-100.0f * cos(m_elapsedTime), 1000.f, -1000.0f * sin(m_elapsedTime), 1.0f); // spinning sun
		sp.cameraPos	= currentCamera->getPosition();

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

		m_baseMeshEffect.bindTexture("grassTexture", m_grassTexture.getTexture());
		m_baseMeshEffect.bindTexture("rockTexture", m_rockTexture.getTexture());

		// Clear and render objects
		Renderer::clearScreen();
		Renderer::renderMesh(cam, m_terrain.getMesh(), m_baseMeshEffect);

		// Render skybox last
		if (currentCamera != &m_orthoCam) m_skybox.renderSkybox(cam);
	
	}

	virtual void onImGuiRender() override 
	{
	
		ImGui::Begin("Rush 2 debug window");
		if (ImGui::Button("Toggle Third person camera")) {

			if (currentCamera == &m_player.getCamera())
				currentCamera = &m_orthoCam;
			else
				currentCamera = &m_player.getCamera();
		}

		if (ImGui::DragFloat("Zoom : ", &aspectRatio, 1.0f, 2.0f, 200) 	)
		{
			m_orthoCam.setProjection<OrthographicProjection>(OrthographicProjection{ -aspectRatio , aspectRatio, aspectRatio, -aspectRatio, 0.0, 10000 });
		}

		ImGui::DragFloat("Sun strength : ", &sp.sunStrength.vector4_f32[0], 0.05f, 0.1F, 2.F);
		if (ImGui::DragFloat3("Camera pos : ", &m_orthocamPos.vector4_f32[0], 0.5f)) {
			m_orthoCam.setPosition(m_orthocamPos);
		}

		ImGui::End();
		m_player.onImGuiRender();
		m_terrain.showDebugWindow();
	
	}


};
