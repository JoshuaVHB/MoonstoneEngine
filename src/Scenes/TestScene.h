#pragma once

#include "Scene.h"
#include "../../Graphics/Rendering_impl/direct3D11_impl.h"

#include "../../Graphics/World/Cube.h"
#include "../../Graphics/abstraction/Camera.h"
#include "..\..\Graphics\abstraction\Effect.h"
#include "../../Graphics/abstraction/Framebuffer.h"
#include "../../Graphics/World/Mesh.h"
#include "../../Graphics/World/Cube.h"
#include "../../Graphics/World/Player.h"
#include "../../Graphics/World/WorldRendering/Skybox.h"

#include "../../Platform/IO/Inputs.h"
#include "../../Platform/WindowsEngine.h"
#include <iostream>
#include <memory>
#include <directXmath.h>

#include "../../Graphics/World/Material.h"
#include "../../Graphics/World/Frustum.h"
#include "../../Graphics/World/Managers/MeshManager.h"

#include "../../Utils/Transform.h"

#include <PxPhysicsAPI.h>

#define DRAGFLOAT(flt) ImGui::DragFloat(#flt, &flt, 1,-100,100);


using namespace physx;

class TestScene : public Scene {

private:

	float dt = 0 , elapsed = 0;

	Mesh ball, cube, bunny;
	Effect renderShader, blitFx;
	Texture breadbug = Texture(L"res/textures/breadbug.dds");
	Skybox box;

	Transform transform;
	AABB aabb;

	Framebuffer fbo;

	struct worldParams {
		// la matrice totale 
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
		XMMATRIX u_MVP;
	};

	Texture bb = Texture(L"res/textures/breadbug.dds");
	Player m_player;
	Material m;
	bool renderSponza = false;

	Camera lastcam;

	// -- Define physx objects
	PxScene*				gScene		= NULL;
	PxDefaultAllocator		gAllocator;
	PxDefaultErrorCallback	gErrorCallback;
	PxFoundation*			gFoundation = NULL;
	PxPhysics*				gPhysics	= NULL;
	PxDefaultCpuDispatcher* gDispatcher = NULL;
	PxPvd*					gPvd		= NULL;

	void initPhysics() {
		gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

		// For debugging physx
		gPvd = PxCreatePvd(*gFoundation);
#define PVD_HOST "127.0.0.1"
		PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
		gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

		gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

		PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
		sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
		gDispatcher = PxDefaultCpuDispatcherCreate(2);
		sceneDesc.cpuDispatcher = gDispatcher;

		gScene = gPhysics->createScene(sceneDesc);

		PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
		if (pvdClient)
		{
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}
	}

	void cleanupPhysics(bool /*interactive*/)
	{
		PX_RELEASE(gScene);
		PX_RELEASE(gDispatcher);
		PX_RELEASE(gPhysics);
		if (gPvd)
		{
			PxPvdTransport* transport = gPvd->getTransport();
			gPvd->release();	gPvd = NULL;
			PX_RELEASE(transport);
		}
		PX_RELEASE(gFoundation);
	}

public:

	TestScene() 
	{
		ball = MeshManager::loadMeshFromFile("res/mesh/blenderCube.obj");
		bunny = MeshManager::loadMeshFromFile("res/mesh/sponza.obj");

		renderShader.loadEffectFromFile("res/effects/baseMesh.fx");
		blitFx.loadEffectFromFile("res/effects/blit.fx");

		renderShader.addNewCBuffer("worldParams", sizeof(worldParams));
		renderShader.addNewCBuffer("meshParams", sizeof(meshParams));

		InputLayout testlayout;
		testlayout.pushBack<3>(InputLayout::Semantic::Position);
		testlayout.pushBack<3>(InputLayout::Semantic::Normal);
		testlayout.pushBack<2>(InputLayout::Semantic::Texcoord);
		auto tmplayout = testlayout.asInputDesc();

		renderShader.bindInputLayout(testlayout);
		m.loadTextures(
			{
				{"res/textures/Sphere_Base_Color.dds", TextureType::ALBEDO}		
			
			}
		);

		lastcam = m_player.getCamera();
		
	}

	virtual void onPhysicSimulation(float deltaTime) override {
		gScene->simulate(deltaTime);
		gScene->fetchResults(true);
	}

	virtual void onUpdate(float deltaTime) override
	{
		dt = deltaTime;
		elapsed += deltaTime;

		m_player.step(elapsed);

		aabb.origin = transform.getPosition();
		aabb.size = transform.getScale();


		worldParams sp;

		sp.viewProj = XMMatrixTranspose(m_player.getCamera().getVPMatrix());
		sp.lightPos = XMVectorSet(-10.0f * cos(elapsed), 10.0f * sin(elapsed), -10.0f, 1.0f);
		sp.cameraPos = m_player.getCamera().getPosition();
		sp.ambiantLight = XMVectorSet(.2f, 0.2f, 0.2f, 1.0f);
		sp.diffuseLight = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		sp.ambiantMat = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		sp.diffuseMat = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);

		renderShader.updateSubresource(sp, "worldParams");
		renderShader.sendCBufferToGPU("worldParams");

	}

	virtual void onRender() override {
	
		if (!renderSponza) Framebuffer::unbind();
		if (renderSponza)
		{
		}

		Renderer::clearScreen();

		Camera& cam = m_player.getCamera();
		Frustum f = Frustum::createFrustumFromPerspectiveCamera(cam);


		renderShader.bindTexture("tex", bb.getTexture());

		//Renderer::renderMesh(cam, ball, renderShader);
		//Renderer::renderMesh(cam, bunny, renderShader);

		if (renderSponza) Renderer::renderDebugPerspectiveCameraOutline(cam, lastcam);

		Renderer::renderAABB(cam, aabb);
		box.renderSkybox(cam);


		

	}
	virtual void onImGuiRender() override {
		ImGui::Begin("Debug");

		ImGui::Text(std::to_string(m_player.getCamera().getPosition().vector4_f32[0]).c_str());

		std::string hello = "no";
		Frustum f = Frustum::createFrustumFromPerspectiveCamera(m_player.getCamera());
		if (f.isOnFrustum(aabb))
		{
			hello = "yes";
		}
		ImGui::Text(hello.c_str());

		bunny.getTransform().showControlWindow();
		transform.showControlWindow();

		ImGui::Checkbox("render sponza", &renderSponza);
		if (ImGui::Button("photo")) {
			lastcam = m_player.getCamera();
		}
		ImGui::End();

		m_player.onImGuiRender();
		Renderer::showImGuiDebugData();
	}


};
