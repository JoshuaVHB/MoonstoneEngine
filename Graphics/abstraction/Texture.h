#pragma once

#include <filesystem>
#include <utility>
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
	ID3D11ShaderResourceView* m_texture = nullptr;
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

	void swap(Texture& other) {
		std::swap(other.m_path, m_path);
#ifdef D3D11_IMPL
		std::swap(other.m_texture, m_texture);
		m_renderContext = WindowsEngine::getInstance().getGraphics().getContext();
#endif
	}

	Texture(const Texture& other) 
		: m_texture(other.m_texture)
		, m_path(other.m_path)
		, m_renderContext(WindowsEngine::getInstance().getGraphics().getContext())
	{}

	Texture& operator=(const Texture& other) {
		Texture{ other }.swap(*this);
		return *this;

	}

	Texture(Texture&& other) noexcept
		: m_path(std::exchange(other.m_path, L""))
#ifdef D3D11_IMPL
		, m_renderContext(other.m_renderContext)
		, m_texture(std::exchange(other.m_texture, nullptr))
#endif
	{	}
	Texture& operator=(Texture&& other) noexcept
	{
		Texture{ std::move(other) }.swap(*this);
		return *this;
	}


	~Texture() {

		
		DX_RELEASE(m_texture);
	}

	ID3D11ShaderResourceView* getTexture() const { return m_texture; }

};