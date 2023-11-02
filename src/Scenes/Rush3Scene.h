#pragma once

#include "Scene.h"

#include <filesystem>
#include "../../Graphics/World/WorldRendering/Terrain.h"
#include "../../Graphics/abstraction/Camera.h"
#include "../../Graphics/abstraction/FrameBuffer.h"
#include "../../Physics/World/PhysicalHeightMap.h"
#include "../../Physics/World/DynamicObject.h"
#include "../../Physics/physx_Impl/physx_shape.h"
class Rush3Scene : public Scene {


private:

	// -- Terrain
	const std::filesystem::path path_to_map = "res/textures/heightmap.png";
	Terrain m_terrain{ path_to_map };
	Vec		centerPoint; // for camera target
	Texture m_rockTexture{ L"res/textures/rock.dds" };
	Texture m_grassTexture{ L"res/textures/seamless.dds" };
	Texture m_grass6Texture{ L"res/textures/breadbug.dds" };
	Texture m_snow{ L"res/textures/snow.dds" };

	PhysicalHeightMap phm;

	// -- Effect and skybox
	Effect	m_baseMeshEffect;
	Skybox  m_skybox;

	// -- Camera
	Player  m_player;					// Basically the 1st person camera
	Camera	m_orthoCam;
	Camera* currentCamera = &m_player.getCamera();
	Vec m_orthocamPos = { 0,120,50 };	// For imgui purposes
	float aspectRatio = 100;

	// -- hot reload of the heightmap
	std::filesystem::file_time_type ftime = std::filesystem::last_write_time(path_to_map);
	std::filesystem::file_time_type lastTime;
	float m_elapsedTime = 0;

	FrameBuffer fbo;
	bool m_disableAABBS = false;


	// -- Meshes
	Mesh bunny;
	DynamicObject cube_P;

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

	Rush3Scene()
	{
		// -- Import the baseMesh effect
		m_baseMeshEffect.loadEffectFromFile("res/effects/terrain.fx");



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
			OrthographicProjection{ -aspectRatio , aspectRatio, aspectRatio, -aspectRatio, 0.0,  100 + 5 * aspectRatio }
		);
		m_orthoCam.setPosition(m_orthocamPos);
		Terrain::TerrainParams& p = m_terrain.getParams();
			
		m_terrain.rebuildMesh();
		centerPoint = { (p.width * p.xyScale) / 2, 0.f, (p.height * p.xyScale) / 2};
		m_orthoCam.updateCam();
		m_orthoCam.lookAt(centerPoint);

		phm.setTerrain(&m_terrain);


		bunny = MeshManager::loadMeshFromFile("res/mesh/bunny.obj");
		bunny.getTransform().setPosition({ +150, 200, +120 });
		cube_P.setMesh(&bunny);
		cube_P.addShape(physx_shape::getBall());
		
		


		Renderer::setBackbufferToDefault();
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
		sp.viewProj = XMMatrixTranspose(currentCamera->getVPMatrix());
		sp.lightPos = XMVectorSet(-100.0f, 1000.f, -1000.0f, 1.0f); // sun ?
		sp.cameraPos = currentCamera->getPosition();

		m_baseMeshEffect.updateSubresource(sp, "worldParams");
		m_baseMeshEffect.sendCBufferToGPU("worldParams");

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

		m_baseMeshEffect.bindTexture("grassTexture", m_grassTexture.getTexture());
		m_baseMeshEffect.bindTexture("rockTexture", m_rockTexture.getTexture());
		m_baseMeshEffect.bindTexture("snowTexture", m_snow.getTexture());

		// Clear and render objects
		Renderer::clearScreen();
		for (auto&& m : m_terrain.getMesh())
		{
			// Don't do this ...
			if (currentCamera == &m_orthoCam)
			{
				Renderer::renderMesh(cam, m, m_baseMeshEffect);
				continue;

			}

			if (f.isOnFrustum(m.getBoundingBox()))
			{
				Renderer::renderMesh(cam, m, m_baseMeshEffect);
				if (!m_disableAABBS)
					Renderer::renderAABB(cam, m.getBoundingBox());
			}
		}


		cube_P.updateTransform();
		Renderer::renderMesh(cam, *cube_P.getMesh(), m_baseMeshEffect);



		// Render skybox last (if first person)
		if (currentCamera == &m_player.getCamera()) m_skybox.renderSkybox(cam);

		

	}

	virtual void onImGuiRender() override
	{

		ImGui::Begin("Rush 3 debug window");

		if (ImGui::Button("Switch camera")) {

			if (currentCamera == &m_player.getCamera())
				currentCamera = &m_orthoCam;
			else
				currentCamera = &m_player.getCamera();
		}

		ImGui::Checkbox("Disable AABBs (10 draw calls per box... so slow !)", &m_disableAABBS);


		if (ImGui::DragFloat("Orthograhic zoom : ", &aspectRatio, 1.0f, 2.0f, 200))
		{
			Terrain::TerrainParams& p = m_terrain.getParams();
			centerPoint = { (p.width * p.xyScale) / 2, 0.f, (p.height * p.xyScale) / 2 };
			float distanceToCenter = XMVectorGetX(XMVector3Length(XMVectorSubtract(centerPoint, m_orthoCam.getPosition())));

			m_orthoCam.setProjection<OrthographicProjection>(
				OrthographicProjection{ -aspectRatio , aspectRatio, aspectRatio, -aspectRatio, 0.0,  100 + distanceToCenter*2 * aspectRatio/10}
			);
		}

		ImGui::DragFloat("Sun strength : ", &sp.sunStrength.vector4_f32[0], 0.05f, 0.1F, 2.F);
		if (ImGui::DragFloat3("Orthograhic camPos : ", &m_orthocamPos.vector4_f32[0], 0.5f)) {
			m_orthoCam.setPosition(m_orthocamPos);
		}

		ImGui::End();
		m_player.onImGuiRender();
		m_terrain.showDebugWindow();
		Renderer::showImGuiDebugData();

	}

};
