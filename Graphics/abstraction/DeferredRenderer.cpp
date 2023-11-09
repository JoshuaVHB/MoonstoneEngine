﻿#include "DeferredRenderer.h"
#include "Camera.h"

#include "Renderer.h"
#include "World/Material.h"
#include "World/WorldRendering/Skybox.h"

DeferredRenderer::DeferredRenderer()
	{
		/*	Normal
		 *	Albedo
		 *	Position
		 *	Specular
		 *	AO
		 *	Roughness
		 *	+ depth
		 */



		m_gBuffer = FrameBuffer{ 6 };


		m_gPass.loadEffectFromFile("res/effects/gPass_2.fx");
		m_lightPass.loadEffectFromFile("res/effects/lightPass_2.fx");
		// add vfx pass


		//-------------------------------------------------------------------------------------------//

		InputLayout vertexLayout;
		vertexLayout.pushBack<3>(InputLayout::Semantic::Position);
		vertexLayout.pushBack<3>(InputLayout::Semantic::Normal);
		vertexLayout.pushBack<2>(InputLayout::Semantic::Texcoord);




		m_lightPass.addNewCBuffer("cameraParams", sizeof(cameraParams)); // we need VP and camera pos
		m_lightPass.addNewCBuffer("lightsBuffer", sizeof(hlsl_GenericLight) * 128); // we need VP and camera pos

		m_gPass.addNewCBuffer("cameraParams", sizeof(XMMATRIX)); // we only need VP matrix
		m_gPass.addNewCBuffer("meshParams", sizeof(meshParams));

		m_gPass.bindInputLayout(vertexLayout);


		//-------------------------------------------------------------------------------------------//


		m_lights.addPointlight(PointLight{
			Vec{5,5,5,5},
			Vec{0,10,0,0},
			Vec{0,1,1,0},
			Vec{1,1,1,0},
			0.4f, true
			});

	}


	void DeferredRenderer::update(Camera& cam)
	{

		updateGeometryPass(cam);
		updateLightPass(cam);
		// VFX PASS
	}


	void DeferredRenderer::renderDeferred(const std::function<void()>& renderFn, Camera& cam) const
	{
		computeGeometryPass(renderFn);
		combineAndApplyLights(cam);
	}

	/* DeferredRenderer m_renderer;
	 *
	 *
	 *	m_renderer.RenderDeferred(
	 *		[&](){
	 *
	 *		};
	 *
	 *
	 *	);
	 *
	 */

	void DeferredRenderer::renderMesh(Camera& cam, const Mesh& mesh)
	{

		if (!mesh.getMaterials().empty())
		{

			Material mat = mesh.getMaterials()[0];
			m_gPass.bindTexture("ambiantOcclusion", mat.queryTexture<TextureType::AO>());
			m_gPass.bindTexture("albedo", mat.queryTexture<TextureType::ALBEDO>());
			m_gPass.bindTexture("normalMap", mat.queryTexture<TextureType::NORMAL>());
			m_gPass.bindTexture("roughness", mat.queryTexture<TextureType::ROUGHNESS>());
			m_gPass.bindTexture("specular", mat.queryTexture<TextureType::SPECULAR>());
		}
		Renderer::renderMesh(cam, mesh, m_gPass);
	}

	void DeferredRenderer::renderSkybox(Camera& cam, const Skybox& skybox)
	{
		m_skyboxSRV = m_gBuffer.bindUnlitRTV();
		skybox.renderSkybox(cam);
	}



	void DeferredRenderer::updateLightPass(Camera& cam)
	{

		m_lightPass.updateSubresource(cameraParams{
			cam.getVPMatrix(), cam.getPosition()
			}, "cameraParams");
		m_lightPass.sendCBufferToGPU("cameraParams");

	}
	void DeferredRenderer::updateGeometryPass(Camera& cam) const
	{

		m_gPass.updateSubresource(XMMatrixTranspose(cam.getVPMatrix()), "cameraParams");
		m_gPass.sendCBufferToGPU("cameraParams");

	}

	/* Fills the gBuffer with the scene informations */
	// Use deferredRenderer.renderMesh()
	void DeferredRenderer::computeGeometryPass(const std::function<void()>& renderFn) const
	{
		m_gBuffer.bind();
		renderFn();
		m_gBuffer.unbind();
	}

	void DeferredRenderer::combineAndApplyLights(Camera& camera) const
	{

		uploadLights();

		m_lightPass.bindTexture("normal", m_gBuffer.getResource(0));
		m_lightPass.bindTexture("albedo", m_gBuffer.getResource(1));
		m_lightPass.bindTexture("position", m_gBuffer.getResource(2));
		m_lightPass.bindTexture("specular", m_gBuffer.getResource(3));
		m_lightPass.bindTexture("ambiantOcclusion", m_gBuffer.getResource(4));
		m_lightPass.bindTexture("roughness", m_gBuffer.getResource(5));
		m_lightPass.bindTexture("unlitTexture", m_skyboxSRV);
		m_lightPass.apply();

		Renderer::draw(6);
		m_lightPass.unbindResources();

	}


	void DeferredRenderer::uploadLights() const
	{
		std::vector<hlsl_GenericLight> toUpload;
		toUpload = m_lights.convertLights();
		toUpload.resize(128);

		m_lightPass.updateSubresource(toUpload.data()[0], "lightsBuffer");
		m_lightPass.sendCBufferToGPU("lightsBuffer");
	}



	void DeferredRenderer::showDebugWindow()
	{

		ImGui::Begin("Deferred Shading debug window");

		for (ID3D11ShaderResourceView* srv : m_gBuffer.getShaderResourceViews())
		{
			ImGui::Image((void*)srv, ImVec2(311, 173));
		}
		ImGui::Image((void*)m_skyboxSRV, ImVec2(311, 173));
		ImGui::End();
		m_lights.showDebugWindow();

	}
