#pragma once

#include "Scene.h"
#include "../../Graphics/Rendering_impl/direct3D11_impl.h"

#include "../../Graphics/World/Cube.h"
#include "../../Graphics/abstraction/Camera.h"
#include "..\..\Graphics\abstraction\Effect.h"
#include "../../Graphics/abstraction/FrameBuffer.h"
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


#define DRAGFLOAT(flt) ImGui::DragFloat(#flt, &flt, 1,-100,100);

#include "../../Physics/World/DynamicObject.h"

#include "../../Platform/IO/JsonParser.h"

using namespace physx;

class TestScene : public Scene {

private:

	float dt = 0 , elapsed = 0;

	Mesh bunny;

	std::vector<Mesh> meshes{};

	Effect renderShader, blitFx, gPassFx, lightPassFx;
	Texture breadbug = Texture(L"res/textures/breadbug.dds");
	Skybox box;



	Transform transform;
	AABB aabb;

	FrameBuffer fbo{3};

	struct worldParams {
		// la matrice totale 
		XMMATRIX viewProj;
		XMVECTOR lightPos; // la position de la source d��clairage (Point)
		XMVECTOR cameraPos; // la position de la cam�ra 
		XMVECTOR ambiantLight; // la valeur ambiante de l��clairage
		XMVECTOR diffuseLight; // la valeur ambiante du mat�riau 
		XMVECTOR ambiantMat; // la valeur diffuse de l��clairage 
		XMVECTOR diffuseMat; // la valeur diffuse du mat�riau 
	};

	struct meshParams {
		XMMATRIX worldMat;
	};

	Texture bb = Texture(L"res/textures/breadbug.dds");
	Player m_player;
	Material m;
	bool renderSponza = false;

	Camera lastcam;

	DynamicObject cube_P;

	std::vector<PhysicalObject> cubes{};

	JsonParser parser{ "res/json/Position.json" };

	std::vector<FormatJson> objs = parser.getObjs();

	

public:

	TestScene() 
	{
		bunny = MeshManager::loadMeshFromFile("res/mesh/bunny.obj");

		std::for_each(objs.begin(), objs.end(), [&](FormatJson& obj) {
			meshes.push_back(MeshManager::loadMeshFromFile(obj.pathObj));
		});

		cube_P.setMesh(&bunny);		

		renderShader.loadEffectFromFile("res/effects/baseMesh.fx");
		lightPassFx.loadEffectFromFile("res/effects/lightPass.fx");
		gPassFx.loadEffectFromFile("res/effects/gPass.fx");
		blitFx.loadEffectFromFile("res/effects/blit.fx");

		renderShader.addNewCBuffer("worldParams", sizeof(worldParams));
		renderShader.addNewCBuffer("meshParams", sizeof(meshParams));

		InputLayout testlayout;
		testlayout.pushBack<3>(InputLayout::Semantic::Position);
		testlayout.pushBack<3>(InputLayout::Semantic::Normal);
		testlayout.pushBack<2>(InputLayout::Semantic::Texcoord);

		renderShader.bindInputLayout(testlayout);
		m.loadTextures(	{	{"res/textures/Sphere_Base_Color.dds", TextureType::ALBEDO}	}	);



		lightPassFx.addNewCBuffer("worldParams", sizeof(worldParams));
		gPassFx.addNewCBuffer("worldParams", sizeof(worldParams));
		gPassFx.addNewCBuffer("meshParams", sizeof(meshParams));
		gPassFx.bindInputLayout(testlayout);



		lastcam = m_player.getCamera();		
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

		gPassFx.updateSubresource(sp, "worldParams");
		gPassFx.sendCBufferToGPU("worldParams");

		lightPassFx.updateSubresource(sp, "worldParams");
		lightPassFx.sendCBufferToGPU("worldParams");


	}

	virtual void onRender() override {
	
		Renderer::clearScreen();
		Camera& cam = m_player.getCamera();
		if (renderSponza)
		{
			fbo.clear();
			fbo.bind();

			Frustum f = Frustum::createFrustumFromPerspectiveCamera(cam);

			gPassFx.bindTexture("tex", bb.getTexture());
			Renderer::renderMesh(cam, bunny, gPassFx);
			bunny.getTransform().setPosition({ 0,0,0 });

			setPosObjs(cam);

			auto SRVLit = fbo.bindUnlitRTV();
			box.renderSkybox(cam);			

			fbo.unbind();

			lightPassFx.bindTexture("tex", bb.getTexture());
			lightPassFx.bindTexture("normal", fbo.getResource(0));
			lightPassFx.bindTexture("albedo", fbo.getResource(1));
			lightPassFx.bindTexture("position", fbo.getResource(2));
			lightPassFx.bindTexture("unlitTexture", SRVLit);
			lightPassFx.apply();
			Renderer::draw(6);


		}
		else {

			Renderer::clearScreen();

			//Renderer::renderAABB(cam, aabb);
			cube_P.updateTransform();

			Renderer::renderMesh(cam, *(cube_P.getMesh()), renderShader);

			setPosObjs(cam);

			box.renderSkybox(cam);
				Camera& cam = m_player.getCamera();
				Frustum f = Frustum::createFrustumFromPerspectiveCamera(cam);
				renderShader.bindTexture("tex", bb.getTexture());
				Renderer::renderMesh(cam, bunny, renderShader);
				box.renderSkybox(cam);

		}

		

	}
	virtual void onImGuiRender() override {
		ImGui::Begin("Debug");
		ImGui::Text("MY BELOVED TEST SCENE !!! THIS IS HIGHLY EXPERIMENTAL");
		bunny.getTransform().showControlWindow();
		transform.showControlWindow();

		ImGui::Checkbox("USE DEFERRED RENDERING", &renderSponza);
		ImGui::End();

		m_player.onImGuiRender();
		Renderer::showImGuiDebugData();
	}

	void setPosObjs(Camera cam) {
		for (int i = 0; i < objs.size(); i++) {
			Renderer::renderMesh(cam, meshes[i], gPassFx);
			meshes[i].getTransform().setPosition(objs[i].positionObj);
			meshes[i].getTransform().setRotation(objs[i].forwardObj);
			meshes[i].getTransform().setScale(objs[i].scaleObj);
		}
	}


};
