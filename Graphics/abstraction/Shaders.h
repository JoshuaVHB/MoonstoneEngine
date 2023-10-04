#pragma once

#include <iostream>
#include <filesystem>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <effects.h>

#include "../../Utils/Debug.h"

#include "../../d3dx11effect.h"
#include "Sampler.h"
#include "Texture.h"
#include <unordered_set>
#include "../../Platform/IO/DDSTextureLoader11.h"
#include "../../Platform/WindowsEngine.h"

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

struct ShaderLayout 
{


};


class Effect 
{
private:

	Graphics::RenderingContext m_renderContext;
	ID3DX11EffectTechnique* m_technique; 

	D3D11_INPUT_ELEMENT_DESC layout[3] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,	0,		D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,		16,		D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,	32,		D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

public:
	ID3DX11EffectPass* m_pass;
	ID3D11InputLayout* m_vertexLayout = nullptr;
	ID3D11Buffer* pConstantBuffer;
	ID3DX11Effect* m_effect;

	// tmp
	ID3D11SamplerState* pSampleState;
	ID3D11ShaderResourceView* pTextureD3D;

	std::unordered_map<std::string, Sampler> m_samplers;

	Effect()
	{
#ifdef D3D11_IMPL
		m_renderContext = WindowsEngine::getInstance().getGraphics().getContext();
#endif
	}

	void loadEffectFromFile(const fs::path& pathToEffect) {

		// Création d’un tampon pour les constantes du VS 
		D3D11_BUFFER_DESC bd; 
		ZeroMemory( &bd, sizeof(bd) );
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(ShadersParams); 
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0; 
		m_renderContext.device->CreateBuffer( &bd, NULL, &pConstantBuffer );



		ID3DBlob* blob = nullptr;


		ID3DBlob* compilationErrorMessage;
		HRESULT hr = D3DCompileFromFile(pathToEffect.c_str(),
			0, 0, 0, "fx_5_0", 0, 0, &blob, &compilationErrorMessage);

		if (hr != S_OK) {
			const void* errorBuffer = compilationErrorMessage->GetBufferPointer();
			const char* errorMessage = reinterpret_cast<const char*>(errorBuffer);
			throw std::runtime_error(errorMessage);
		}
		D3DX11CreateEffectFromMemory(
			blob->GetBufferPointer(), blob->GetBufferSize(), 0, m_renderContext.device, &m_effect);

		blob->Release();

		m_technique = m_effect->GetTechniqueByIndex(0); 
		m_pass = m_technique->GetPassByIndex(0);

		// Créer l’organisation des sommets pour le VS de notre effet 
		D3DX11_PASS_SHADER_DESC effectVSDesc; 
		m_pass->GetVertexShaderDesc(&effectVSDesc);
		D3DX11_EFFECT_SHADER_DESC effectVSDesc2; 

		effectVSDesc.pShaderVariable->GetShaderDesc(effectVSDesc.ShaderIndex, &effectVSDesc2); 
		const void *vsCodePtr = effectVSDesc2.pBytecode; 
		unsigned vsCodeLen = effectVSDesc2.BytecodeLength; 
		m_vertexLayout = NULL;
		DX_TRY_CODE( m_renderContext.device->CreateInputLayout(layout, ARRAYSIZE(layout), vsCodePtr, vsCodeLen, &m_vertexLayout), 13);


		////////////////
		CreateDDSTextureFromFile(m_renderContext.device, L"res/textures/breadbug.dds", nullptr, &pTextureD3D);

		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		// Création de l’état de sampling
		m_renderContext.device->CreateSamplerState(&samplerDesc, &pSampleState);

		
	}

	void apply() const {

		m_pass->Apply(0, m_renderContext.context);

		ID3DX11EffectSamplerVariable* variableSampler;
		variableSampler = m_effect->GetVariableByName("SampleState")->AsSampler();
		//variableSampler = tmp->AsSampler();
		variableSampler->SetSampler(0, pSampleState);

	}

	void bindTexture(std::string uniformName, ID3D11ShaderResourceView* tex)
	{
		m_effect->GetVariableByName(uniformName.c_str())->AsShaderResource()->SetResource(tex);
	}



};