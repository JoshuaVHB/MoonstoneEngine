#pragma once

#include "../Cube.h"
#include "../Mesh.h"
#include "../../abstraction/Shaders.h"
#include "../../abstraction/Camera.h"
#include "../../abstraction/TextureCube.h"

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
	}


	void renderSkybox(Camera& camera) 
	{
		m_params.viewProj = camera.getVPMatrix();
		m_skyboxPass.apply();
		m_skyboxPass.updateSubresource(m_params, "SkyboxCbuffer");
		m_skyboxPass.sendCBufferToGPU("SkyboxCbuffer");
		//Renderer::drawIndexed()
	}

};