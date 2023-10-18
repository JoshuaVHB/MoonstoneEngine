///////////////////////////////////////
/// THIS FILE NEED LOTS OF CLEANING	///
///	BE PATIENT PLS					///
///////////////////////////////////////

#pragma once

#include <filesystem>
#include <vector>
#include <d3d11.h>
#include <string>

#include "../../d3dx11effect.h"
#include "Sampler.h"
#include <unordered_map>
#include <map>
#include "../../Platform/WindowsEngine.h"

#define _XM_NO_INTRINSICS_
#include <DirectXMath.h>

using namespace DirectX;
namespace fs = std::filesystem;

struct ShadersParams {
	// la matrice totale 
	XMMATRIX matWorldViewProj; 
	XMMATRIX matWorld; // matrice de transformation dans le monde
	XMVECTOR vLumiere; // la position de la source d��clairage (Point)
	XMVECTOR vCamera; // la position de la cam�ra 
	XMVECTOR vAEcl; // la valeur ambiante de l��clairage
	XMVECTOR vAMat; // la valeur ambiante du mat�riau 
	XMVECTOR vDEcl; // la valeur diffuse de l��clairage 
	XMVECTOR vDMat; // la valeur diffuse du mat�riau 
};

struct InputLayout 
{

private:


	LPCSTR semantics_str[9] = {
		"BINORMAL",
		"BLENDINDICES",
		"BLENDWEIGHT",
		"NORMAL",
		"POSITION",
		"POSITIONT",
		"PSIZE",
		"TANGENT",
		"TEXCOORD"
	};

	UINT stride = 0;

	const std::vector<DXGI_FORMAT> formats{
		DXGI_FORMAT_R32_FLOAT,
		DXGI_FORMAT_R32G32_FLOAT,
		DXGI_FORMAT_R32G32B32_FLOAT,
		DXGI_FORMAT_R32G32B32A32_FLOAT
	};

public:

	
	enum class Semantic {
		Binormal,
		BlendIndices,
		BlendWeight, 
		Normal,
		Position,
		PositionT,
		PSize,
		Tangent,
		Texcoord
	};

	std::vector<D3D11_INPUT_ELEMENT_DESC> elems;

	D3D11_INPUT_ELEMENT_DESC* desc = nullptr;
	ID3D11InputLayout* inputLayout = nullptr;

	InputLayout() = default;

	D3D11_INPUT_ELEMENT_DESC* asInputDesc() {
		return elems.data();
	}

	// Works for now, as i'm not sure we will really use anything else than float32(s) formats
	template<size_t floatCount>
	void pushBack(Semantic elemType)
	{
		elems.push_back(
			D3D11_INPUT_ELEMENT_DESC{
				semantics_str[size_t(elemType)], 0,
				formats[floatCount-1], 
				0, stride,
				D3D11_INPUT_PER_VERTEX_DATA, 0
			}
		);
		stride += 16;
	}
};


class Effect 
{
private:

	d3d11_graphics::RenderingContext m_renderContext;
	ID3DX11EffectTechnique* m_technique; 

	std::map < std::string, ID3D11Buffer* > m_constantBuffers;

	ID3DX11EffectPass* m_pass;
	ID3D11InputLayout* m_vertexLayout = nullptr;
	ID3DX11Effect* m_effect;

	// tmp
	ID3D11SamplerState* pSampleState;
	std::unordered_map<std::string, Sampler> m_samplers;

public:

	Effect();
	ID3D11InputLayout* getVertexLayout() const { return m_vertexLayout; }


	void loadEffectFromFile(const fs::path& pathToEffect);
	void bindInputLayout(InputLayout layout);
	void createSampler();

	// Returns the slot number of the newly created cbuffer
	void addNewCBuffer(const std::string& name, uint32_t structSize);
	void apply() const;
	void bindTexture(std::string uniformName, ID3D11ShaderResourceView* tex);

	template<class ShaderParam>
	void updateSubresource(const ShaderParam& sp, const std::string& cbuffName) const
	{
		if (!m_constantBuffers.contains(cbuffName)) throw;
		m_renderContext.context->UpdateSubresource(m_constantBuffers.at(cbuffName), 0, nullptr, &sp, 0, 0);
	}

	void sendCBufferToGPU(const std::string& cbuffName) const;


};