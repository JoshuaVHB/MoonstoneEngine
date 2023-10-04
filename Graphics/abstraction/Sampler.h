#pragma once

#include <filesystem>
#include <d3d11.h>

#include "../../Utils/Debug.h"

class Sampler {

	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_context = nullptr;

public:

	Sampler(ID3D11Device* device = nullptr,
	ID3D11DeviceContext* context = nullptr) {


		m_device = device;
		m_context = context;
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
		m_device->CreateSamplerState(&samplerDesc, &pSampleState);

	}


	void bind() {


	}

private:

	ID3D11ShaderResourceView* pTextureD3D;
	ID3D11SamplerState* pSampleState;





};