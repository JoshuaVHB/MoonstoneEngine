#pragma once

#include <filesystem>
#include <d3d11.h>
#include <array>

#include "../../Utils/Debug.h"
#include "../../Platform/IO/DDSTextureLoader11.h"
#include "../../Platform/WindowsEngine.h"

namespace fs = std::filesystem;
class TextureCube {

private:

	std::string m_path;

#ifdef D3D11_IMPL
	ID3D11ShaderResourceView* m_texture;
	d3d11_graphics::RenderingContext m_renderContext;
	std::array<ID3D11ShaderResourceView*, 6> m_texs;
#endif

public:

	TextureCube() {}

	TextureCube(const std::string& path)
		: m_path(path), m_texture(nullptr)
	{
#ifdef D3D11_IMPL
		m_renderContext = WindowsEngine::getInstance().getGraphics().getContext();
		//CreateDDSTextureFromFile(m_renderContext.device, path.c_str(), nullptr, &m_texture);


		for (int i = 0; i < 6; i++) 
		{
			std::wstring p = L"res/textures/" + std::to_wstring(i) + L".dds";
			DirectX::CreateDDSTextureFromFile(m_renderContext.device, p.c_str(), nullptr, &m_texs[i]);
		}

		D3D11_TEXTURE2D_DESC desc{};

		desc.Width = 450;
		desc.Height= 450;
		desc.MipLevels= 1;
		desc.ArraySize= 6;
		desc.Format= DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		D3D11_SUBRESOURCE_DATA data[6];

		for (int i = 0; i < 6; i++) {

			data[i].pSysMem = m_texs[i];
			data[i].SysMemPitch = 0; // TODO
			data[i].SysMemSlicePitch= 0;
		}

		ID3D11Texture2D* ptex;
		m_renderContext.device->CreateTexture2D(&desc, data, &ptex);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};

		srvDesc.Format = desc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		m_renderContext.device->CreateShaderResourceView(ptex, &srvDesc, &m_texture);

#endif
	}


};