#include "DeferredRenderer.h"
#include "Camera.h"

#include "Renderer.h"
#include "World/Frustum.h"
#include "World/Material.h"
#include "World/WorldRendering/Skybox.h"
#include "World/WorldRendering/Terrain.h"

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


		m_gPass.loadEffectFromFile("res/effects/gPass.fx");
		m_lightPass.loadEffectFromFile("res/effects/lightPass.fx");
		m_deferredTerrainFx.loadEffectFromFile("res/effects/deferredTerrain.fx");
		// add vfx pass

		//-------------------------------------------------------------------------------------------//

		InputLayout vertexLayout;
		vertexLayout.pushBack<3>(InputLayout::Semantic::Position);
		vertexLayout.pushBack<3>(InputLayout::Semantic::Normal);
		vertexLayout.pushBack<2>(InputLayout::Semantic::Texcoord);

		m_lightPass.addNewCBuffer("cameraParams", sizeof(cameraParams)); // we need VP and camera pos
		m_lightPass.addNewCBuffer("lightsBuffer", sizeof(hlsl_GenericLight) * 128); // we need VP and camera pos

		m_gPass.addNewCBuffer("cameraParams", sizeof(cameraParams)); // we only need VP matrix
		m_gPass.addNewCBuffer("meshParams", sizeof(meshParams));

		m_deferredTerrainFx.addNewCBuffer("meshParams", sizeof(meshParams));
		m_deferredTerrainFx.addNewCBuffer("cameraParams", sizeof(cameraParams));

		m_gPass.bindInputLayout(vertexLayout);
		m_deferredTerrainFx.bindInputLayout(vertexLayout);

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

	void DeferredRenderer::renderMesh(Camera& cam, const Mesh& mesh) const
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

	void DeferredRenderer::renderTerrain(Camera& cam, Terrain& terrain) const
	{

		static Texture m_rockTexture{ L"res/textures/rock.dds" };
		static Texture m_grassTexture{ L"res/textures/seamless.dds" };
		static Texture m_snow{ L"res/textures/snow.dds" };
		static Texture m_rockTexture_normal{ L"res/textures/rock_normal.dds" };
		static Texture m_grassTexture_normal{ L"res/textures/grass_normal.dds" };
		static Texture m_snow_normal{ L"res/textures/snow_normal.dds" };

		m_deferredTerrainFx.bindTexture("grassTexture", m_grassTexture.getTexture());
		m_deferredTerrainFx.bindTexture("rockTexture", m_rockTexture.getTexture());
		m_deferredTerrainFx.bindTexture("snowTexture", m_snow.getTexture());
		m_deferredTerrainFx.bindTexture("snow_normal", m_snow_normal.getTexture());
		m_deferredTerrainFx.bindTexture("grass_normal", m_grassTexture_normal.getTexture());
		m_deferredTerrainFx.bindTexture("rock_normal", m_rockTexture_normal.getTexture());

		m_deferredTerrainFx.updateSubresource(cameraParams{
			XMMatrixTranspose(cam.getVPMatrix()), cam.getPosition()
			}, "cameraParams");
		m_deferredTerrainFx.sendCBufferToGPU("cameraParams");
		Frustum f = Frustum::createFrustumFromPerspectiveCamera(cam);
		for (auto&& chunk : terrain.getMesh())
		{
			if (f.isOnFrustum(chunk.getBoundingBox()))
			{
				Renderer::renderMesh(cam, chunk, m_deferredTerrainFx);
			}
		}	

	}

	void DeferredRenderer::renderSkybox(Camera& cam, const Skybox& skybox) const
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

		m_gPass.updateSubresource(cameraParams{
			XMMatrixTranspose(cam.getVPMatrix()), cam.getPosition()
			}, "cameraParams");
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
		ImGui::Begin("Deferred Controls");
		static float sunstrenght = 1.f;
		static float sunPos[4] = {};
		if (ImGui::DragFloat("Sun strength", &sunstrenght, 0.05f, 0.f, 2.f))
		{
			m_lightPass.setUniformVector("sunStrength", { sunstrenght });
		}

		if (ImGui::DragFloat4("Sun dir", &sunPos[0]))
		{
			m_lightPass.setUniformVector("sunDir", { sunPos[0],sunPos[1] ,sunPos[2] ,sunPos[3] });
		}
		ImGui::End();
		
		m_lights.showDebugWindow();

	}
