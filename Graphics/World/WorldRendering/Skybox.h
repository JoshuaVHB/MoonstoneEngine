#pragma once

#include "../Cube.h"
#include "../Mesh.h"
#include "../../abstraction/Shaders.h"
#include "../../abstraction/Camera.h"
#include "../../abstraction/TextureCube.h"

#include "../../Renderer.h"

#include <string>

class Skybox {


private:

	struct SkyboxParam {
		XMMATRIX viewProj;
		XMVECTOR camPos;
	} m_params;
	Mesh	m_mesh		= Cube::getCubeMesh();
	Effect	m_skyboxPass;


	TextureCube m_tex{ "res/textures/ex.dds" };



public:

	Skybox() {

		m_skyboxPass.loadEffectFromFile("res/effects/skybox.fx");
		
		m_skyboxPass.addNewCBuffer("SkyboxCbuffer", sizeof(SkyboxParam));
		//m_mesh.m_worldMat = XMMatrixTranslationFromVector({ 1,0,0 });


		InputLayout testlayout;
		testlayout.pushBack<3>(InputLayout::Semantic::Position);
		m_skyboxPass.bindInputLayout(testlayout);

	}

	const Mesh& getMesh() const { return m_mesh; }


	void renderSkybox(Camera& camera) 
	{
		m_skyboxPass.bindTexture("tex", m_tex.getResourceView());

		m_params.viewProj = XMMatrixTranspose(camera.getVPMatrix());
		m_params.camPos = camera.getPosition();
		m_skyboxPass.updateSubresource(m_params, "SkyboxCbuffer");
		m_skyboxPass.sendCBufferToGPU("SkyboxCbuffer");
		m_skyboxPass.apply();
		
		Renderer::renderCubemap(camera, m_mesh, m_skyboxPass);
	}

};