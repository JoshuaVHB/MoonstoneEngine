#pragma once

#include "Scene.h"

#include <filesystem>
#include "../../Graphics/World/WorldRendering/Terrain.h"
#include "../../Graphics/abstraction/Camera.h"
#include "../../Physics/World/PhysicalHeightMap.h"
#include "../../Physics/World/TriggerBox.h"
#include "../Game/CameraController.h"
#include "../Game/CheckpointController.h"
#include "../Game/Cloporte.h"
#include "abstraction/DeferredRenderer.h"
#include "abstraction/2D/UI/UIRenderer.h"

class Rush3Scene : public Scene {


private:

	JsonParser m_parser{ "res/json/Position.json" };
	std::vector<FormatJson> m_objs = m_parser.getObjs();
	std::vector<Mesh> m_meshes{};


	Cloporte clop;
	DeferredRenderer m_renderer;

	// -- Terrain
	Terrain m_terrain{ "res/textures/heightmap.png" };
	PhysicalHeightMap phm;


	// -- Effect and skybox
	Skybox  m_skybox;

	// -- Camera
	Camera* currentCamera = nullptr;

	float m_elapsedTime = 0;

	//Pause
	// sadly i have not done inputs that well so WM_CHARDOWN is not correct
	bool m_isPaused = false;
	bool m_hasEscBeenReleased = false;
	Texture m_screenShot;
	FrameBuffer m_fbo;


	Texture m_screenShot;

	// -- Objs and JsonParser
	JsonParser m_parser{ "res/json/Position.json" };
	std::vector<FormatJson> m_objs = m_parser.getObjs();
	std::vector<Mesh> m_meshes{};

	// Checkpoints
	CheckpointController checkpoints;
	TriggerBox* finish;

private:

	// -- Define constant buffers
	struct worldParams {
		XMMATRIX viewProj;
		XMVECTOR lightPos;
		XMVECTOR cameraPos;
		XMVECTOR sunColor = { 1.f,1.f,.8f,1.f };
		XMVECTOR sunStrength = { 0.75f };
	} sp;

	struct meshParams {
		XMMATRIX worldMat;
	};

public:

	Rush3Scene() :
		clop{}
	{
	
		phm.setTerrain(static_cast<const Terrain*>(&m_terrain));
		CameraController::setTerrain(static_cast<const Terrain*>(&m_terrain));
		currentCamera = &clop.getCurrentCamera();		
		clop.setPosition(+530.f , + 60.f, +960.f);
		Renderer::setBackbufferToDefault();

		std::for_each(m_objs.begin(), m_objs.end(), [&](FormatJson& obj) {
			m_meshes.push_back(MeshManager::loadMeshFromFile(obj.pathObj));
		});

		// -- Import the checkpoint
		std::vector<FormatJson> checkpointInfos;
		JsonParser cp_parser{ "res/json/Checkpoints.json" };
		cp_parser.openFile();
		cp_parser.getCheckpoints(checkpointInfos);
		
		checkpoints.addCheckpointFromJson(checkpointInfos);
	
		// -- Set the finish line
		finish = new TriggerBox{ { 240.f, 55.f, 440.f }, { 1, 30, 45, } };
		finish->setTriggerCallback([&]() {
			if(checkpoints.allCheckpointsPassed())
				std::cout << "You win !" << std::endl;
			else 
				std::cout << "You didn't pass all checkpoints !" << std::endl;
			});
			
		UIRenderer::attachMouse(wMouse.get());
	}
	
	void handleKeyboardInputsMenu() {

		if (!wKbd->isKeyPressed(VK_ESCAPE)) m_hasEscBeenReleased = true;
		if (wKbd->isKeyPressed(VK_ESCAPE) && m_hasEscBeenReleased)
		{
			m_hasEscBeenReleased = false;
			m_isPaused = !m_isPaused;
		}


		if (wKbd->isKeyPressed(Keyboard::letterCodeFromChar('r')))
		{

			clop.setTranslation(+530.f, +40.f, +960.f);
			m_elapsedTime = 0;
		}
	}


	virtual void onUpdate(float deltaTime) override
	{
		handleKeyboardInputsMenu();
		PhysicsEngine::setRunningState(!m_isPaused);
		if (m_isPaused) return;

		m_elapsedTime += deltaTime;
		Camera& cam = *currentCamera;

		// Get the ground normal if we are close to the ground
		XMVECTOR groundNormal = (XMVectorGetY(clop.getPosition()) - m_terrain.getWorldHeightAt(clop.getPosition()) < 5.f) 
			? m_terrain.getNormalAt(clop.getPosition()) 
			: XMVECTOR{0, 1, 0, 0};
		
		clop.setGroundVector(groundNormal);
		clop.update(deltaTime);
		m_renderer.update(cam);


	}

	virtual void onRender() override {

		// Get our target camera
		Renderer::clearScreen();
		Renderer::clearText();
		Camera& cam = *currentCamera;
		Frustum f = Frustum::createFrustumFromPerspectiveCamera(cam);
		const auto winSize = WindowsEngine::getInstance().getGraphics().getWinSize();

		// Clear and render objects
		if (m_isPaused) m_fbo.bind();
		m_renderer.clear();
		m_renderer.renderDeferred([&]() -> void
		{
				m_renderer.renderTerrain(cam, m_terrain);
				m_renderer.renderMesh(cam, clop.getMesh());
				m_renderer.renderSkybox(cam, m_skybox);

		}, cam);



		if (m_isPaused)
		{
			m_fbo.unbind();
			Renderer::setBackbufferToDefault();
			m_screenShot = Texture(m_fbo.getResource(0));
			Renderer::blitTexture(m_screenShot, DirectX::XMVECTOR{ 1,1,1,0.5 });
			Renderer::writeTextOnScreen("Pause", 0, 0, 1);
		}


	}

	virtual void onImGuiRender() override
	{

		Renderer::writeTextOnScreen(
			std::string("Fasts/h : ") + std::to_string(static_cast<int>((clop.getObject().getLinearVelocityMag() / clop.getMaxVelocity()) * 100)),
			300, -300, 1);

		// Timer
		if (m_elapsedTime < 60)
			//Afficher le temps en secondes et ms
			Renderer::writeTextOnScreen(
				std::string("Time : ") + std::to_string(static_cast<int>(m_elapsedTime) % 60)
				+ std::string("s ")
				+ std::to_string(static_cast<int>(m_elapsedTime * 1000) % 1000) + std::string("ms"),
				-615, -300, 1);
		else
			//Afficher le temps en minutes et secondes et ms
			Renderer::writeTextOnScreen(std::string("Time : ") +
				std::to_string(static_cast<int>(m_elapsedTime) / 60) + std::string("m ") +
				std::to_string(static_cast<int>(m_elapsedTime) % 60) + std::string("s ") +
				std::to_string(static_cast<int>(m_elapsedTime * 1000) % 1000) +
				std::string("ms"),
				-600, -300, 1);

		Renderer::renderText();
		if (UIRenderer::Button(0, 0, 100, 50))
		{
	
			clop.setTranslation(+530.f, +40.f, +960.f);
			m_elapsedTime = 0;
			
		}
		UIRenderer::renderUI();
		static float tmp = 0;
		
		m_terrain.showDebugWindow();
		Renderer::showImGuiDebugData();
		m_renderer.showDebugWindow();
		 
	}

};
