#pragma once

#include "Scene.h"

#include "../../Graphics/World/Cube.h"
#include "../../Graphics/abstraction/Camera.h"
#include "../../Graphics/abstraction/Shaders.h"
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
#include "../../Graphics/World/Managers/MeshManager.h"



class TestScene : public Scene {

private:

	float dt = 0 , elapsed = 0;

	Mesh ball, cube, bunny;
	Effect renderShader, blitFx;
	Texture breadbug = Texture(L"res/textures/breadbug.dds");
	Skybox box;

	Framebuffer fbo;

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
		XMMATRIX u_MVP;
	};

	Texture bb = Texture(L"res/textures/breadbug.dds");
	Player m_player;
	Material m;
	bool renderSponza = false;

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
		
	}


	virtual void onUpdate(float deltaTime) override
	{
		dt = deltaTime;
		elapsed += deltaTime;

		m_player.step(elapsed);
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
			fbo.bind();
		}
		


		Renderer::clearScreen();

		Camera& cam = m_player.getCamera();
		renderShader.bindTexture("tex", bb.getTexture());
		//Renderer::renderMesh(cam, ball, renderShader);
		Renderer::renderMesh(cam, bunny, renderShader);
		box.renderSkybox(cam);

		//////////////

		/*
		auto tex = fbo.getTex();
		renderShader.bindTexture("tex", tex.getTexture());
		blitFx.apply();
		WindowsEngine::getInstance().getGraphics().getContext().context->Draw(6, 0);
		*/

		

	}
	virtual void onImGuiRender() override {
		ImGui::Begin("Debug");

		ImGui::Text(std::to_string(m_player.getCamera().getPosition().vector4_f32[0]).c_str());
		ImGui::End();
		ImGui::Checkbox("render sponza", &renderSponza);

		m_player.onImGuiRender();

	}


};
