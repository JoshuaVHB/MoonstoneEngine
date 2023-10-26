#pragma once
#include "Camera.h"
#include "Effect.h"
#include "Framebuffer.h"
#include "Renderer.h"
#include "../World/Lights/LightManager.h"
#include "World/Material.h"
#include "World/WorldRendering/Skybox.h"

class Mesh;


struct meshParams
{
	XMMATRIX worldMat;
};
struct cameraParams
{
	XMMATRIX viewProj;
	XMVECTOR cameraPos; // la position de la caméra 
};

class DeferredRenderer
{
private:

	Framebuffer m_gBuffer;

	Effect m_gPass, m_lightPass, m_VFXPass;


	LightManager m_lights;
	ID3D11ShaderResourceView* m_skyboxSRV = nullptr;
	Texture breadbug;
public:

	DeferredRenderer()
	{
		/*	Normal
		 *	Albedo
		 *	Position
		 *	Specular
		 *	AO
		 *	Roughness
		 *	+ depth
		 */

		

		m_gBuffer = Framebuffer{ 6 };


		m_gPass.loadEffectFromFile("res/effects/gPass_2.fx");
		m_lightPass.loadEffectFromFile("res/effects/lightPass_2.fx");
		// add vfx pass
		breadbug = Texture(L"res/textures/breadbug.dds");

		
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

	
		m_lights.addPointlight(PointLight {
			Vec{5,5,5,5},
			Vec{0,10,0,0},
			Vec{0,1,1,0},
			Vec{1,1,1,0},
			0.4f, true
			});

	}


	void update(Camera& cam)
	{

		updateGeometryPass(cam);
		updateLightPass(cam);
		// VFX PASS
	}


	void renderDeferred(const std::function<void()>& renderFn, Camera& cam) const 
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

	void renderMesh(Camera& cam, const Mesh&  mesh)
	{

		if (!mesh.getMaterials().empty())
		{
				
			Material mat = mesh.getMaterials()[0];
			m_gPass.bindTexture("ambiantOcclusion", mat.queryTexture<TextureType::AO>()	);
			m_gPass.bindTexture("albedo", mat.queryTexture<TextureType::ALBEDO>()		);
			m_gPass.bindTexture("normalMap", mat.queryTexture<TextureType::NORMAL>()	);
			m_gPass.bindTexture("roughness", mat.queryTexture<TextureType::ROUGHNESS>()	);
			m_gPass.bindTexture("specular", mat.queryTexture<TextureType::SPECULAR>()	);
		}
		m_gPass.bindTexture("tex", breadbug);
		Renderer::renderMesh(cam, mesh, m_gPass);
	}

	void renderSkybox(Camera& cam, Skybox& skybox)
	{
		m_skyboxSRV = m_gBuffer.bindUnlitRTV();
		skybox.renderSkybox(cam);
	}

	void clear() const noexcept { m_gBuffer.clear(); }

	Texture getLastTexture() const { return {}; };


	void updateLightPass(Camera& cam)
	{

		m_lightPass.updateSubresource(cameraParams{
			cam.getVPMatrix(), cam.getPosition()
		}, "cameraParams");
		m_lightPass.sendCBufferToGPU("cameraParams");

	}
	void updateGeometryPass(Camera& cam) const
	{

		m_gPass.updateSubresource(XMMatrixTranspose(cam.getVPMatrix()), "cameraParams");
		m_gPass.sendCBufferToGPU("cameraParams");

	}

	/* Fills the gBuffer with the scene informations */
	// Use deferredRenderer.renderMesh()
	void computeGeometryPass(const std::function<void()>& renderFn) const
	{
		m_gBuffer.bind();
		renderFn();
	}

	void combineAndApplyLights(Camera& camera) const
	{


		Renderer::setBackbufferToDefault();

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

	}


	void uploadLights() const 
	{
		std::vector<hlsl_GenericLight> toUpload;
		toUpload = m_lights.convertLights();
		toUpload.resize(128);

		m_lightPass.updateSubresource(toUpload.data()[0], "lightsBuffer");
		m_lightPass.sendCBufferToGPU("lightsBuffer");
	}



	void showDebugWindow()
	{

		ImGui::Begin("Deferred Shading debug window");

		for (ID3D11ShaderResourceView* srv : m_gBuffer.get_srvs())
		{			
			ImGui::Image((void*)srv, ImVec2(311 ,173 ));
		}
		ImGui::Image((void*)m_skyboxSRV, ImVec2(311 ,173 ));
		ImGui::End();
		m_lights.showDebugWindow();

	}
};
