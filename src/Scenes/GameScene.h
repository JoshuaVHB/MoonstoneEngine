#pragma once

#include "Scene.h"

#include <filesystem>

#include "Renderer.h"
#include "../../Graphics/World/WorldRendering/Terrain.h"
#include "../../Graphics/abstraction/Camera.h"
#include "../../Physics/World/PhysicalHeightMap.h"
#include "../../Physics/World/TriggerBox.h"
#include "../Game/CameraController.h"
#include "../Game/CheckpointController.h"
#include "../Game/Cloporte.h"
#include "abstraction/DeferredRenderer.h"
#include "abstraction/2D/UI/UIRenderer.h"

class GameScene : public Scene {


private:

	JsonParser m_parser{ "res/json/Position.json" };
	std::vector<FormatJson> m_objs = m_parser.getObjs();
	std::vector<Mesh> m_meshes{};


	Cloporte m_player;
	DeferredRenderer m_renderer;

	// -- Terrain
	Terrain m_terrain{ "res/textures/heightmap.png" };
	PhysicalHeightMap phm;


	// -- Effect and skybox
	Skybox  m_skybox {"res/textures/garden.dds"};

	// -- Camera
	Camera* currentCamera = nullptr;


	bool isGameOver = false;
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
	std::vector<TriggerBox*> m_borders;
	bool m_isInBound = true;



public:

	GameScene() 
	{
		importObjects();

		createFinishLine();
		initDeathBox();			
		MoveToLastCheckpoint();

		// Create terrain collisions
		phm.setTerrain(&m_terrain);
		CameraController::setTerrain(&m_terrain);

		// Attach mouse for UI (will move somewhere else)
		UIRenderer::attachMouse(wMouse.get());

		currentCamera = &m_player.getCurrentCamera();		
		Renderer::setBackbufferToDefault();
	}
	


	virtual void onUpdate(float deltaTime) override
	{
		// Handle pausing
		handleKeyboardInputsMenu();
		PhysicsEngine::setRunningState(!m_isPaused); 
		if (m_isPaused) return;

		// Has the player gone out of bounds ? (...)
		if (!m_isInBound)
		{
			MoveToLastCheckpoint();
			m_isInBound = true;
		}

		// Increase the timer if the run is still going
		if (!isGameOver) m_elapsedTime += deltaTime;

		// Current camera is the current player view (FPS/TPS)
		currentCamera = &m_player.getCurrentCamera();
		Camera& cam = *currentCamera;



		// Get the ground normal if we are close to the ground (used for basic camera mouvement and ground checks, this will move)
		const XMVECTOR groundNormal = (XMVectorGetY(m_player.getPosition()) - m_terrain.getWorldHeightAt(m_player.getPosition()) < 5.f) 
			? m_terrain.getNormalAt(m_player.getPosition()) 
			: XMVECTOR{{0.F, 1.f, 0.F, 0.f}};
		m_player.setGroundVector(groundNormal);

		// Finished run effect
		if (isGameOver) { currentCamera->lookAt(m_player.getPosition()); }

		// Update player and deferred renderer (it is not static yet)
		m_player.update(deltaTime);
		m_renderer.update(cam);

	}

	virtual void onRender() override {

		// Clear everything
		Renderer::clearScreen();
		Renderer::clearText();
		m_renderer.clear();


		// Render objects 
		Camera& cam = *currentCamera;
		if (m_isPaused || isGameOver) m_fbo.bind(); // < Used to darken the screen
		m_renderer.renderDeferred([&]() -> void
		{
				m_renderer.renderTerrain(cam, m_terrain); // < This has frustum culling
				m_renderer.renderMesh(cam, m_player.getMesh());
				m_renderer.renderSkybox(cam, m_skybox);

		}, cam);


		// Darken the screen if its a pause or we have finished
		if (m_isPaused || isGameOver)
		{
			m_fbo.unbind();
			Renderer::setBackbufferToDefault();
			m_screenShot = Texture(m_fbo.getResource(0));
			Renderer::blitTexture(m_screenShot, DirectX::XMVECTOR{{1.F,1.F,1.F,0.5F}});

			if (isGameOver)			Renderer::writeTextOnScreen("YOU WIN !!!!!!!!!!", 0, 0, 1);
			if (m_isPaused)			Renderer::writeTextOnScreen("Pause", 0, 0, 1);
		}

	}

	virtual void onImGuiRender() override
	{

		std::stringstream speedText{}, timeText{};
		const auto winSize = WindowsEngine::getInstance().getGraphics().getWinSize();
		
		speedText << "Fasts/h " << static_cast<int>((m_player.getObject().getLinearVelocityMag() / m_player.getMaxVelocity()) * 100);
		timeText << "Time : " << static_cast<int>(m_elapsedTime / 60) << "m " << static_cast<int>(m_elapsedTime) % 60 << "s " << static_cast<int>(m_elapsedTime * 1000) % 1000 << "ms";

		Renderer::writeTextOnScreen(speedText.str(), (winSize.first / 2)-300, -(winSize.second / 2) + 50, 1);
		Renderer::writeTextOnScreen(timeText.str(),-(winSize.first / 2), - (winSize.second/2)+50, 1);

		Renderer::renderText();
		UIRenderer::renderUI();

#ifdef _DEBUG
		m_terrain.showDebugWindow();
		Renderer::showImGuiDebugData();
		m_renderer.showDebugWindow();
		ImGui::Begin("InGame scene debug");
		ImGui::Text("X : %f   Y : %f    Z : %f", XMVectorGetX(m_player.getPosition()), XMVectorGetY(m_player.getPosition()), XMVectorGetZ(m_player.getPosition()));
		ImGui::End();
#endif
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Game logic methods, this will be moved somewhere soon !
private:

	void initDeathBox()
	{
		const float scale  = m_terrain.getParams().xyScale;
		const float length = static_cast<int>(m_terrain.getParams().height) * scale;
		const float width  = static_cast<int>(m_terrain.getParams().width) * scale;
		const float height = m_terrain.getHeightmap().getMaxHeight() * m_terrain.getParams().scaleFactor;

		m_borders.push_back(new TriggerBox{ PhysicalObject::fVec3{ length / 2.f, -1.f, width / 2.f}, PhysicalObject::fVec3{length, 1.f, width} });
		m_borders.push_back(new TriggerBox{ PhysicalObject::fVec3{0.f, height / 2.f, width / 2.f}, PhysicalObject::fVec3{ 1.f, 1.5f * height, width / 1.5f} });
		m_borders.push_back(new TriggerBox{ PhysicalObject::fVec3{length / 2.f, height / 2.f, 0.f}, PhysicalObject::fVec3{ length / 1.5f, 1.5f * height, 1.f} });
		m_borders.push_back(new TriggerBox{ PhysicalObject::fVec3{length, height / 2.f, width / 2.f}, PhysicalObject::fVec3{ 1.f, 1.5f * height, width / 1.5f} });
		m_borders.push_back(new TriggerBox{ PhysicalObject::fVec3{length / 2.f, height / 2.f, width}, PhysicalObject::fVec3{ length / 1.5f, 1.5f * height, 1.f} });


		std::ranges::for_each(m_borders, [&](TriggerBox* border) {
			border->setTriggerCallback([&]() {
				m_isInBound = false;
				});
			});

	}

	void createFinishLine()
	{
		// -- Set the finish line
		finish = new TriggerBox{ { 240.f, 55.f, 440.f }, { 1, 30, 45, } };
		finish->setTriggerCallback([&]() {
			EndGame();
			});
	}

	void MoveToLastCheckpoint() {
		const XMVECTOR t = checkpoints.getPositionLastCP();
		m_player.setTranslation(DirectX::XMVectorGetX(t), DirectX::XMVectorGetY(t), DirectX::XMVectorGetZ(t));
		m_player.setForward(checkpoints.getDirectionLastCP());
	}

	void EndGame() {
		if (checkpoints.allCheckpointsPassed())
		{
			std::cout << "You win !" << std::endl;
			currentCamera = &m_player.getThirdPersonCam();
			currentCamera->setPosition(XMVECTOR{{230.F , 100.F , 400.f , 0.F}});
			currentCamera->lookAt(m_player.getPosition());
			currentCamera->updateCam();
			m_player.setPlayable(false);
			isGameOver = true;
		}
		else
			std::cout << "You need to pass all checkpoint !" << std::endl;
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
			m_player.setPlayable(true);
			if (isGameOver) {
				isGameOver = false;
				checkpoints.resetAllCheckpoints();
				currentCamera->updateCam();
				m_elapsedTime = 0;
			}
			MoveToLastCheckpoint();
		}
	}

	[[maybe_unused]] void importObjects()
	{
		std::vector<FormatJson> checkpointInfos;
		JsonParser cp_parser{ "res/json/Checkpoints.json" };
		cp_parser.openFile();
		cp_parser.getCheckpoints(checkpointInfos);

		checkpoints.addCheckpointFromJson(checkpointInfos);


		std::ranges::for_each(m_objs, [&](const FormatJson& obj) {
			m_meshes.push_back(MeshManager::loadMeshFromFile(obj.pathObj));
			});

	}

};
