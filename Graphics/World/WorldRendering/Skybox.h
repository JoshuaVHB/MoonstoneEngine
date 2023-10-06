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

	struct SkyboxParam {XMMATRIX viewProj; } m_params;
	Mesh	m_mesh		= Cube::getCubeMesh();
	Effect	m_skyboxPass;


	TextureCube m_tex{ "" };



public:

	Skybox() {

		m_skyboxPass.loadEffectFromFile("res/effects/skybox.fx");
		
		m_skyboxPass.addNewCBuffer("SkyboxCbuffer", sizeof(SkyboxParam));
		m_mesh.m_worldMat = XMMatrixTranslationFromVector({ 1,0,0 });
	}


	void renderSkybox(Camera& camera) 
	{
		m_skyboxPass.apply();
		m_params.viewProj = XMMatrixTranspose(camera.getVPMatrix());
		m_skyboxPass.updateSubresource(m_params, "SkyboxCbuffer");
		m_skyboxPass.sendCBufferToGPU("SkyboxCbuffer");
		
		Renderer::renderCubemap(camera, m_mesh, m_skyboxPass);
	}

};