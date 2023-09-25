#pragma once

#include <iostream>
#include <filesystem>
#include <d3d11.h>
#include <d3dcompiler.h>

#include "../../Utils/Debug.h"

namespace fs = std::filesystem;

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


};