#pragma once

#include "Scene.h"

#include <filesystem>
#include "../../Graphics/World/WorldRendering/Terrain.h"
#include "../../Graphics/abstraction/Camera.h"
#include "../../Graphics/abstraction/FrameBuffer.h"
#include "../../Physics/World/PhysicalHeightMap.h"
#include "../../Physics/World/DynamicObject.h"
#include "../../Physics/World/TriggerBox.h"
#include "../../Physics/physx_Impl/physx_shape.h"
#include "../Game/CameraController.h"

#include "../Game/Cloporte.h"
class Rush3Scene : public Scene {


private:

	Cloporte clop;

	// -- Terrain
	const std::filesystem::path path_to_map = "res/textures/heightmap.png";
	Terrain m_terrain{ path_to_map };
	Vec		centerPoint; // for camera target
	Texture m_rockTexture{ L"res/textures/rock.dds" };
	Texture m_grassTexture{ L"res/textures/seamless.dds" };
	Texture m_snow{ L"res/textures/snow.dds" };

	PhysicalHeightMap phm;

	// -- Effect and skybox
	Effect	m_terrainEffect;
	Skybox  m_skybox;

	// -- Camera
	Camera* currentCamera = nullptr;

	// -- hot reload of the heightmap
	std::filesystem::file_time_type lastTime,  ftime = std::filesystem::last_write_time(path_to_map);
	float m_elapsedTime = 0;

	bool m_disableAABBS = true;


	// -- Meshes
	TriggerBox start;

	Texture m_screenShot;



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
		start{PhysicsEngine::FilterGroup::eFinish},
		clop{}
	{
		// -- Import the baseMesh effect
		m_terrainEffect.loadEffectFromFile("res/effects/terrain.fx");
		m_terrainEffect.addNewCBuffer("worldParams", sizeof(worldParams)); // For camera, light, colors...
		m_terrainEffect.addNewCBuffer("meshParams", sizeof(meshParams)); // For model matrix basically



		// -- Specify how the input vertices are layered
		InputLayout testlayout;
		testlayout.pushBack<3>(InputLayout::Semantic::Position);
		testlayout.pushBack<3>(InputLayout::Semantic::Normal);
		testlayout.pushBack<2>(InputLayout::Semantic::Texcoord);
		m_terrainEffect.bindInputLayout(testlayout);

		//m_terrain.getParams().xyScale = 0.50;
		//m_terrain.getParams().scaleFactor = 25;
		m_terrain.rebuildMesh();
		phm.setTerrain(static_cast<const Terrain*>(&m_terrain));
		currentCamera = &clop.getCurrentCamera();		
		clop.setPosition(+498.f , + 40.f, +984.f);
		//clop.setPosition(10,100.f, 10);
		Renderer::setBackbufferToDefault();
		CameraController::setTerrain(static_cast<const Terrain*>(&m_terrain));
	}


	virtual void onUpdate(float deltaTime) override
	{
		m_elapsedTime += deltaTime;

		XMVECTOR groundNormal = (XMVectorGetY(clop.getPosition()) - m_terrain.getWorldHeightAt(clop.getPosition()) < 5.f) 
			? m_terrain.getNormalAt(clop.getPosition()) 
			: XMVECTOR{0, 1, 0, 0};
		
		clop.setGroundVector(groundNormal);
		clop.update(deltaTime);

		sp.viewProj = XMMatrixTranspose(currentCamera->getVPMatrix());
		sp.lightPos = XMVectorSet(-100.0f, 1000.f, -1000.0f, 1.0f); // sun ?
		sp.cameraPos = currentCamera->getPosition();

		m_terrainEffect.updateSubresource(sp, "worldParams");
		m_terrainEffect.sendCBufferToGPU("worldParams");

		// -- Check for hot reload
		lastTime = std::filesystem::last_write_time(path_to_map);
		if (lastTime != ftime) {
			m_terrain.hotReloadMap();
			ftime = lastTime;
		}

	}

	virtual void onRender() override {

		// Get our target camera
		Camera& cam = *currentCamera;
		Frustum f = Frustum::createFrustumFromPerspectiveCamera(cam);

		m_terrainEffect.bindTexture("grassTexture", m_grassTexture.getTexture());
		m_terrainEffect.bindTexture("rockTexture", m_rockTexture.getTexture());
		m_terrainEffect.bindTexture("snowTexture", m_snow.getTexture());

		// Clear and render objects
		Renderer::clearScreen();
		for (auto&& chunk : m_terrain.getMesh())
		{			
			if (f.isOnFrustum(chunk.getBoundingBox()))
			{
				Renderer::renderMesh(cam, chunk, m_terrainEffect);
				if (!m_disableAABBS) Renderer::renderAABB(cam, chunk.getBoundingBox());
			}
		}

		Renderer::renderMesh(cam, clop.getMesh(), m_terrainEffect);		
		Renderer::renderDebugLine(cam, clop.getPosition(), clop.getPosition() + (clop.getGroundDir()*2));
		m_skybox.renderSkybox(cam);		

	}

	virtual void onImGuiRender() override
	{
		static float tmp = 0;
		ImGui::DragFloat4("angles : ", currentCamera->getAngles().vector4_f32);
		ImGui::DragFloat3("direction : ", currentCamera->getForwardDir().vector4_f32);
		ImGui::DragFloat3("player direction : ", clop.getForward().vector4_f32);
		if (ImGui::DragFloat("roll : ", &tmp, 0.05f))
		{
			currentCamera->setRoll(tmp);
			tmp = std::fmod(tmp, DirectX::XM_PI);
		}
		ImGui::Checkbox("Disable AABBs (10 draw calls per box... so slow !)", &m_disableAABBS);
		m_terrain.showDebugWindow();
		Renderer::showImGuiDebugData();

	}

};
