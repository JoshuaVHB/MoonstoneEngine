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
	Skybox  m_skybox {"res/textures/garden.dds"};

	// -- Camera
	Camera* currentCamera = nullptr;

	float m_elapsedTime = 0;

	//Pause
	// sadly i have not done inputs that well so WM_CHARDOWN is not correct
	bool m_isPaused = false;
	bool m_hasEscBeenReleased = false;
	Texture m_screenShot;
	FrameBuffer m_fbo;


	// Checkpoints
	CheckpointController checkpoints;
	TriggerBox* finish;
	bool isGameOver = false;

public:


	void MoveToLastCheckpoint() {
		DirectX::XMVECTOR t = checkpoints.getPositionLastCP();
		clop.setTranslation(DirectX::XMVectorGetX(t), DirectX::XMVectorGetY(t), DirectX::XMVectorGetZ(t));
		clop.setForward(checkpoints.getDirectionLastCP());
	}

	void EndGame() {
		if(checkpoints.allCheckpointsPassed())
		{
			std::cout << "You win !" << std::endl;
			currentCamera = &clop.getThirdPersonCam();
			currentCamera->setPosition({230,100,400});
			currentCamera->lookAt(clop.getPosition());
			currentCamera->updateCam();
			clop.setPlayable(false);
			isGameOver = true;
		}
		else 
			std::cout << "You need to pass all checkpoint !" << std::endl;
	}

public:

	Rush3Scene() :
		clop{}
	{
		// -- Import the checkpoint
		std::vector<FormatJson> checkpointInfos;
		JsonParser cp_parser{ "res/json/Checkpoints.json" };
		cp_parser.openFile();
		cp_parser.getCheckpoints(checkpointInfos);

		checkpoints.addCheckpointFromJson(checkpointInfos);

		// -- Set the finish line
		finish = new TriggerBox{ { 240.f, 55.f, 440.f }, { 1, 30, 45, } };
		finish->setTriggerCallback([&]() {
				EndGame();
			});

		phm.setTerrain(static_cast<const Terrain*>(&m_terrain));
		CameraController::setTerrain(static_cast<const Terrain*>(&m_terrain));
		currentCamera = &clop.getCurrentCamera();		
		
		Renderer::setBackbufferToDefault();

		std::for_each(m_objs.begin(), m_objs.end(), [&](FormatJson& obj) {
			m_meshes.push_back(MeshManager::loadMeshFromFile(obj.pathObj));
		});



		MoveToLastCheckpoint();
			
		UIRenderer::attachMouse(wMouse.get());
	}
	
	void handleKeyboardInputsMenu() {

		if (!wKbd->isKeyPressed(VK_ESCAPE)) m_hasEscBeenReleased = true;
		if (wKbd->isKeyPressed(VK_ESCAPE) && m_hasEscBeenReleased && !isGameOver)
		{
			m_hasEscBeenReleased = false;
			m_isPaused = !m_isPaused;
		}


		if (wKbd->isKeyPressed(Keyboard::letterCodeFromChar('r')))
		{
			clop.setPlayable(true);
			if (isGameOver) {
				isGameOver = false;
				checkpoints.resetAllCheckpoints();
				currentCamera->updateCam();
				m_elapsedTime = 0;
			}
			MoveToLastCheckpoint();
		}
	}


	virtual void onUpdate(float deltaTime) override
	{
		handleKeyboardInputsMenu();
		PhysicsEngine::setRunningState(!m_isPaused);
		if (m_isPaused) return;

		if (!isGameOver) m_elapsedTime += deltaTime;
		Camera& cam = *currentCamera;


		DirectX::XMVECTOR pos = clop.getPosition();

		// Get the ground normal if we are close to the ground
		XMVECTOR groundNormal = (XMVectorGetY(clop.getPosition()) - m_terrain.getWorldHeightAt(clop.getPosition()) < 5.f) 
			? m_terrain.getNormalAt(clop.getPosition()) 
			: XMVECTOR{0, 1, 0, 0};

		if (isGameOver)
		{
			currentCamera->lookAt(clop.getPosition());
		}

		clop.setGroundVector(groundNormal);
		clop.update(deltaTime);
		m_renderer.update(cam);


	}

	virtual void onRender() override {

		DirectX::XMVECTOR pos = clop.getPosition();
		// Get our target camera
		Renderer::clearScreen();
		Renderer::clearText();
		Camera& cam = *currentCamera;
		Frustum f = Frustum::createFrustumFromPerspectiveCamera(cam);
		const auto winSize = WindowsEngine::getInstance().getGraphics().getWinSize();

		// Clear and render objects
		if (m_isPaused || isGameOver) m_fbo.bind();
		m_renderer.clear();
		m_renderer.renderDeferred([&]() -> void
		{
				m_renderer.renderTerrain(cam, m_terrain);
				m_renderer.renderMesh(cam, clop.getMesh());
				m_renderer.renderSkybox(cam, m_skybox);

		}, cam);



		if (m_isPaused || isGameOver)
		{
			m_fbo.unbind();
			Renderer::setBackbufferToDefault();
			m_screenShot = Texture(m_fbo.getResource(0));
			Renderer::blitTexture(m_screenShot, DirectX::XMVECTOR{ 1,1,1,0.5 });

			if (isGameOver)			Renderer::writeTextOnScreen("YOU WIN !!!!!!!!!!", 0, 0, 1);
			if (m_isPaused)			Renderer::writeTextOnScreen("Pause", 0, 0, 1);
		}


	}

	virtual void onImGuiRender() override
	{

		std::stringstream speedText{}, timeText{};
		const auto winSize = WindowsEngine::getInstance().getGraphics().getWinSize();
		
		speedText << "Fasts/h " << static_cast<int>((clop.getObject().getLinearVelocityMag() / clop.getMaxVelocity()) * 100);
		timeText << "Time : " << static_cast<int>(m_elapsedTime / 60) << "m " << static_cast<int>(m_elapsedTime) % 60 << "s " << static_cast<int>(m_elapsedTime * 1000) % 1000 << "ms";

		Renderer::writeTextOnScreen(speedText.str(), (winSize.first / 2)-300, -(winSize.second / 2) + 50, 1);
		Renderer::writeTextOnScreen(timeText.str(),-(winSize.first / 2), - (winSize.second/2)+50, 1);

		Renderer::renderText();
		UIRenderer::renderUI();
		static float tmp = 0;

#ifdef _DEBUG
		m_terrain.showDebugWindow();
		Renderer::showImGuiDebugData();
		m_renderer.showDebugWindow();
#endif
	}

};
