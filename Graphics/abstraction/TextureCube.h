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
	ID3D11ShaderResourceView* m_srv;
	ID3D11Texture2D* m_tex;
	d3d11_graphics::RenderingContext m_renderContext;
#endif

public:

	TextureCube() {}

	TextureCube(const std::string& path)
		: m_path(path), m_srv(nullptr)
	{
#ifdef D3D11_IMPL
		m_renderContext = WindowsEngine::getInstance().getGraphics().getContext();
		DirectX::CreateDDSTextureFromFile(m_renderContext.device, L"res/textures/skybox.dds", nullptr, &m_srv);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};

		srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		m_renderContext.device->CreateShaderResourceView(m_tex, &srvDesc, &m_srv);

#endif
	}


};