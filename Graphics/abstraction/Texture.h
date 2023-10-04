#pragma once

#include <filesystem>
#include <d3d11.h>

#include "../../Utils/Debug.h"
#include "../../Platform/IO/DDSTextureLoader11.h"
#include "../../Platform/WindowsEngine.h"

namespace fs = std::filesystem;
class Texture 
{
private:

	std::wstring m_path;

#ifdef D3D11_IMPL
	ID3D11ShaderResourceView* m_texture;
	Graphics::RenderingContext m_renderContext;
	ID3D11Buffer* m_ibo = nullptr;
#endif

public:

	Texture() {}

	Texture(
		const std::wstring& path
	) 
		: m_path(path), m_texture(nullptr)
	{
#ifdef D3D11_IMPL
		m_renderContext = WindowsEngine::getInstance().getGraphics().getContext();
		CreateDDSTextureFromFile(m_renderContext.device, path.c_str(), nullptr, &m_texture);
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
		//	device->CreateSamplerState(&samplerDesc, &pSampleState);
#endif
	}


	~Texture() {

		DX_RELEASE(m_texture);
	}

	ID3D11ShaderResourceView* getTexture() const { return m_texture; }

};