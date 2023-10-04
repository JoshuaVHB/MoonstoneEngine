#pragma once

#include <filesystem>
#include <d3d11.h>

#include "../../Utils/Debug.h"
#include "../../Platform/IO/DDSTextureLoader11.h"


namespace fs = std::filesystem;
class Texture 

{

private:

	std::wstring m_path;
	ID3D11ShaderResourceView* m_texture;

	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_context = nullptr;

public:

	Texture() {}

	Texture(
		ID3D11Device* device,
		ID3D11DeviceContext* context,
		const std::wstring& path
	) 
		: m_path(path), m_texture(nullptr)
	{
	
		CreateDDSTextureFromFile(device, path.c_str(), nullptr, &m_texture);
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
		// Cr�ation de l��tat de sampling
	//	device->CreateSamplerState(&samplerDesc, &pSampleState);
	}


	~Texture() {

		DX_RELEASE(m_texture);
	}

	ID3D11ShaderResourceView* getTexture() const { return m_texture; }

};