#pragma once

#include "Scene.h"

#include <filesystem>
#include "../../Graphics/World/WorldRendering/Terrain.h"
#include "../../Graphics/abstraction/Camera.h"
#include "../../Physics/World/PhysicalHeightMap.h"
#include "../../Physics/World/TriggerBox.h"
#include "../Game/CameraController.h"

#include "../Game/Cloporte.h"
#include "abstraction/DeferredRenderer.h"

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
	bool m_disableAABBS = true;
	Texture m_screenShot;


	// -- Meshes
	std::vector<TriggerBox*> cp;

public:

	Rush3Scene() :
		clop{}
	{
	
		m_terrain.rebuildMesh();
		phm.setTerrain(static_cast<const Terrain*>(&m_terrain));
		CameraController::setTerrain(static_cast<const Terrain*>(&m_terrain));
		currentCamera = &clop.getCurrentCamera();		
		clop.setPosition(+530.f , + 60.f, +960.f);
		Renderer::setBackbufferToDefault();

		std::for_each(m_objs.begin(), m_objs.end(), [&](FormatJson& obj) {
			m_meshes.push_back(MeshManager::loadMeshFromFile(obj.pathObj));
		});

		auto firstCheckpoint = new TriggerBox{ PhysicsEngine::fVec3(510.f, 40, 840), PhysicsEngine::fVec3(50, 50, 2) };
		firstCheckpoint->setTriggerCallback([]() -> void {
			std::cout << "Checkpoint reached !" << std::endl;
			});
		cp.emplace_back(firstCheckpoint);
	}


	virtual void onUpdate(float deltaTime) override
	{
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
		Camera& cam = *currentCamera;
		Frustum f = Frustum::createFrustumFromPerspectiveCamera(cam);

		// Clear and render objects
		m_renderer.clear();
		m_renderer.renderDeferred([&]() -> void
		{
				m_renderer.renderTerrain(cam, m_terrain);
				m_renderer.renderMesh(cam, clop.getMesh());
				m_renderer.renderSkybox(cam, m_skybox);

		}, cam);
	}

	virtual void onImGuiRender() override
	{
		
		static float tmp = 0;
		
		m_terrain.showDebugWindow();
		Renderer::showImGuiDebugData();
		m_renderer.showDebugWindow();
		 
	}

};
