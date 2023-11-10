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

#include "Renderer.h"
#include "abstraction/Texture.h"
#include "abstraction/2D/Renderer2D.h"
#include "abstraction/2D/UI/UIRenderer.h"

class Rush3Scene : public Scene {


private:

	Cloporte clop;
	float m_speed = 0.0f;

	// -- Terrain
	const std::filesystem::path path_to_map = "res/textures/heightmap2.png";
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
	float m_elapsedTime = 0.0f;

	bool m_disableAABBS = true;


	// -- Meshes
	std::vector<TriggerBox*> cp;

	Texture m_screenShot;

	// -- Objs and JsonParser
	JsonParser m_parser{ "res/json/Position.json" };
	std::vector<FormatJson> m_objs = m_parser.getObjs();
	std::vector<Mesh> m_meshes{};



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
		CameraController::setTerrain(static_cast<const Terrain*>(&m_terrain));
		currentCamera = &clop.getCurrentCamera();		
		clop.setPosition(+530.f , + 60.f, +960.f);
		Renderer::setBackbufferToDefault();

		std::for_each(m_objs.begin(), m_objs.end(), [&](FormatJson& obj) {
			m_meshes.push_back(MeshManager::loadMeshFromFile(obj.pathObj));
		});

		TriggerBox* checkpoint = new TriggerBox{ PhysicsEngine::fVec3(510.f, 40, 840), PhysicsEngine::fVec3(50, 50, 2) };
		checkpoint->setTriggerCallback(std::move([]() {
			std::cout << "Checkpoint reached !" << std::endl;
			}));
		cp.push_back(checkpoint);

		UIRenderer::attachMouse(wMouse.get());
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

		m_speed = clop.getObject().getLinearVelocityMag();

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

		// Vitesse
		Renderer::writeTextOnScreen(std::string("Fasts/h : ") + std::to_string(static_cast<int>((m_speed/clop.getMaxVelocity())*100)), 400, -350, 1);

		// Timer
		if ( m_elapsedTime < 60)
			//Afficher le temps en secondes et ms
			Renderer::writeTextOnScreen(std::string("Time : ") + std::to_string(static_cast<int>(m_elapsedTime)%60) + std::string("s ") + std::to_string(static_cast<int>(m_elapsedTime * 1000) % 1000) + std::string("ms"), -715, -350, 1);
		else
			//Afficher le temps en minutes et secondes et ms
			Renderer::writeTextOnScreen(std::string("Time : ") + std::to_string(static_cast<int>(m_elapsedTime) / 60) + std::string("m ") + std::to_string(static_cast<int>(m_elapsedTime) % 60) + std::string("s ") + std::to_string(static_cast<int>(m_elapsedTime * 1000) % 1000) + std::string("ms"), -700, -350, 1);

		Renderer::renderText();
	}

	virtual void onImGuiRender() override
	{
		UIRenderer::clear();

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

		if (UIRenderer::Button(0, 0, 100, 50))
		{
			if (wMouse->isLeftPressed())
			{
				clop.setPosition(+530.f, +60.f, +960.f);
				m_elapsedTime = 0;
			}
		}

		wMouse->clearPress();

		UIRenderer::renderUI();


		m_terrain.showDebugWindow();
		Renderer::showImGuiDebugData();

	}

	~Rush3Scene() {
		for(auto c : cp)
			delete c;
	}
};