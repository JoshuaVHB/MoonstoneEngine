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
	d3d11_graphics::RenderingContext m_renderContext;
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
		DirectX::CreateDDSTextureFromFile(m_renderContext.device, path.c_str(), nullptr, &m_texture);
#endif
	}


	~Texture() {

		DX_RELEASE(m_texture);
	}

	ID3D11ShaderResourceView* getTexture() const { return m_texture; }

};