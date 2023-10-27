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

#include "World/Lights/Lights.h"

class Texture;
using namespace DirectX;
namespace fs = std::filesystem;

struct ShadersParams {
	// la matrice totale 
	XMMATRIX matWorldViewProj; 
	XMMATRIX matWorld; // matrice de transformation dans le monde
	XMVECTOR vLumiere; // la position de la source d’éclairage (Point)
	XMVECTOR vCamera; // la position de la caméra 
	XMVECTOR vAEcl; // la valeur ambiante de l’éclairage
	XMVECTOR vAMat; // la valeur ambiante du matériau 
	XMVECTOR vDEcl; // la valeur diffuse de l’éclairage 
	XMVECTOR vDMat; // la valeur diffuse du matériau 
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
	UINT instancedStride = 0;

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

	template<size_t floatCount>
	void pushBackInstanced(const char* semanticName)
	{



		elems.push_back(
			D3D11_INPUT_ELEMENT_DESC{
				semanticName,						//     LPCSTR SemanticName;
				0,									//     UINT SemanticIndex;
				formats[floatCount - 1],			//     DXGI_FORMAT Format;
				1,									//     UINT InputSlot;
				instancedStride,					//     UINT AlignedByteOffset;
				D3D11_INPUT_PER_INSTANCE_DATA,		//     D3D11_INPUT_CLASSIFICATION InputSlotClass;
				1									//     UINT InstanceDataStepRate;
			}
		);
		instancedStride += 16;
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
	void bindTexture(const std::string&uniformName, ID3D11ShaderResourceView* tex) const;
	void bindTexture(const std::string& uniformName, const Texture& tex) const;

	template<class ShaderParam>
	void updateSubresource(const ShaderParam& sp, const std::string& cbuffName) const
	{
		if (!m_constantBuffers.contains(cbuffName)) throw std::runtime_error("Couldn't find cbuffer name " + cbuffName);
		m_renderContext.context->UpdateSubresource(m_constantBuffers.at(cbuffName), 0, nullptr, &sp, 0, 0);
	}

	template<class ShaderParam>
	void updateSubresource(const ShaderParam& sp, const std::string& cbuffName, size_t size) const
	{
		if (!m_constantBuffers.contains(cbuffName)) throw std::runtime_error("Couldn't find cbuffer name " + cbuffName);
		m_renderContext.context->UpdateSubresource(m_constantBuffers.at(cbuffName), 0, nullptr, &sp, size, 0);
	}


	template<class cbufferParam>
	void updateCbuffer(const cbufferParam& data, const std::string& cbuffName) const
	{
		D3D11_BUFFER_DESC desc{};
		desc.ByteWidth = 2 *128 * sizeof(PointLight) ;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		ID3D11Buffer* buffer;

		D3D11_SUBRESOURCE_DATA initData{};
		initData.SysMemPitch = desc.ByteWidth;
		initData.SysMemSlicePitch= 0;
		initData.pSysMem = data;

		m_renderContext.device->CreateBuffer(&desc, &initData, &buffer);
		m_effect->GetConstantBufferByName(cbuffName.c_str())->SetConstantBuffer(buffer);
		DX_RELEASE(buffer);
	}


	void sendCBufferToGPU(const std::string& cbuffName) const;

	void unbindResources() const 
	{
		ID3D11ShaderResourceView* nullSRV =  nullptr ;
		m_renderContext.context->PSSetShaderResources(0, 1, &nullSRV);
	}

};