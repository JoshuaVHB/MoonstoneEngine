#pragma once

#define _XM_NO_INTRINSICS_
#include <DirectXMath.h>

#include "Effect.h"
#include "FrameBuffer.h"
#include "../World/Lights/LightManager.h"


class Mesh;
class Camera;
class Skybox;
class Terrain;

struct ID3D11ShaderResourceView;

struct meshParams
{
	DirectX::XMMATRIX worldMat;
};
struct cameraParams
{
	DirectX::XMMATRIX viewProj;
	DirectX::XMVECTOR cameraPos; // la position de la caméra 
};

class DeferredRenderer
{
private:

	FrameBuffer m_gBuffer;

	Effect m_gPass, m_lightPass, m_VFXPass;
	Effect m_deferredTerrainFx;
	LightManager m_lights;
	mutable ID3D11ShaderResourceView* m_skyboxSRV = nullptr;

public:

	void update(Camera& cam);
	void renderDeferred(const std::function<void()>& renderFn, Camera& cam) const;

	void renderTerrain(Camera& cam, Terrain& terrain) const;
	void renderMesh(Camera& cam, const Mesh& mesh)const ;
	void renderSkybox(Camera& cam, const Skybox& skybox)const;

	void clear() const noexcept { m_gBuffer.clear(); }


	void uploadLights() const;
	void showDebugWindow();

private:
	void updateLightPass(Camera& cam);
	void updateGeometryPass(Camera& cam) const;

	/* Fills the gBuffer with the scene informations */
	// Use deferredRenderer.renderMesh()
	void computeGeometryPass(const std::function<void()>& renderFn) const;
	void combineAndApplyLights(Camera& camera) const;


	//////////////////////////////////////////////////////////////////////////////////
	///
public:
	DeferredRenderer();
	DeferredRenderer(const DeferredRenderer&) = delete;
	DeferredRenderer(DeferredRenderer&&) = delete;
	DeferredRenderer& operator=(const DeferredRenderer&) = delete;
	DeferredRenderer& operator=(DeferredRenderer&&) = delete;
	~DeferredRenderer() = default;

};
