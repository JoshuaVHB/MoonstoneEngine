#pragma once

#include <iostream>
#include <filesystem>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <effects.h>

#include "../../Utils/Debug.h"

#include "../../d3dx11effect.h"

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

class Shader 
{
public:

	enum class ShaderType
	{
		VertexShader,
		PixelShader,
	};
private:

	union  {
		ID3D11VertexShader* m_vertexShader;
		ID3D11PixelShader* m_pixelShader;
	} s;

	ShaderType m_type;

	void* m_shader;
	ID3D11InputLayout* pVertexLayout = nullptr;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;

	D3D11_INPUT_ELEMENT_DESC layout[2] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

public:



	Shader(){}
	Shader(ID3D11Device* device,
		ID3D11DeviceContext* context) 
	{
		m_device = device;
		m_context = context;
	}

	void bind() {
		switch (m_type)
		{
		case Shader::ShaderType::VertexShader:
			m_context->IASetInputLayout(pVertexLayout);
			// Activer le VS
			m_context->VSSetShader(s.m_vertexShader, nullptr, 0);
			break;
		case Shader::ShaderType::PixelShader:
			m_context->PSSetShader(s.m_pixelShader, nullptr, 0);
			break;
		default:
			break;
		}
	}
	void loadShaderFromFile(const fs::path& pathToShader, ShaderType type) {

		ID3DBlob* blob = nullptr;
		m_type = type;



		switch (type)
		{
		case Shader::ShaderType::VertexShader:
			m_shader = s.m_vertexShader;

			DX_TRY_CODE(D3DCompileFromFile(pathToShader.c_str(),
				nullptr, nullptr,
				"VS1",
				"vs_5_0",
				D3DCOMPILE_ENABLE_STRICTNESS,
				0,
				&blob, nullptr), 10
			);


			DX_TRY_CODE(m_device->CreateVertexShader(blob->GetBufferPointer(),
				blob->GetBufferSize(),
				nullptr,
				&s.m_vertexShader),
				6);

			// Créer l'organisation des sommets
			pVertexLayout = nullptr;
			DX_TRY_CODE(m_device->CreateInputLayout(layout,
				2,
				blob->GetBufferPointer(),
				blob->GetBufferSize(),
				&pVertexLayout),
				7);

			blob->Release(); //  On n'a plus besoin du blob

			break;
		case Shader::ShaderType::PixelShader:
			m_shader = s.m_pixelShader;

			// Compilation et chargement du pixel shader
			DX_TRY_CODE(D3DCompileFromFile(pathToShader.c_str(),
				nullptr, nullptr,
				"PS1",
				"ps_5_0",
				D3DCOMPILE_ENABLE_STRICTNESS,
				0,
				&blob,
				nullptr), 8);

			DX_TRY_CODE(m_device->CreatePixelShader(blob->GetBufferPointer(),
				blob->GetBufferSize(),
				nullptr,
				&s.m_pixelShader),
				9);

			blob->Release(); //  On n'a plus besoin du blob


			break;
		default:
			break;
		}


	}

};


class Effect 
{
private:

	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;

	ID3DX11EffectTechnique* m_technique; 


	D3D11_INPUT_ELEMENT_DESC layout[2] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

public:
	ID3DX11EffectPass* m_pass;
	ID3D11InputLayout* m_vertexLayout = nullptr;

	ID3DX11Effect* m_effect;
	Effect() {}

	Effect(ID3D11Device* device,
		ID3D11DeviceContext* context)
	{
		m_device = device;
		m_context = context;
	}

	void loadEffectFromFile(const fs::path& pathToEffect) {

		ID3DBlob* blob = nullptr;

		DX_TRY_CODE	(D3DCompileFromFile(pathToEffect.c_str(),
			0, 0, 0, "fx_5_0", 0, 0, &blob, 0),
			11);

		D3DX11CreateEffectFromMemory(
			blob->GetBufferPointer(), blob->GetBufferSize(), 0, m_device, &m_effect);

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
		DX_TRY_CODE( m_device->CreateInputLayout(layout, 2, vsCodePtr, vsCodeLen, &m_vertexLayout), 13);



	}

};