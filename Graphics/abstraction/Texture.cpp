#include "Texture.h"

#include <filesystem>
#include <utility>
#include <d3d11.h>

#include "../../Utils/Debug.h"
#include "../../Platform/IO/DDSTextureLoader11.h"
#include "../../Platform/WindowsEngine.h"

namespace fs = std::filesystem;

Texture::Texture(const std::wstring& path)
: m_path(path), m_texture(nullptr)
{
#ifdef D3D11_IMPL
	m_renderContext = WindowsEngine::getInstance().getGraphics().getContext();
	DirectX::CreateDDSTextureFromFile(m_renderContext.device, path.c_str(), nullptr, &m_texture);
#endif
}

void Texture::swap(Texture& other) noexcept {
	std::swap(other.m_path, m_path);
#ifdef D3D11_IMPL
	std::swap(other.m_texture, m_texture);
	m_renderContext = WindowsEngine::getInstance().getGraphics().getContext();
#endif
}

Texture::Texture(const Texture& other)
	: m_path(other.m_path)
	, m_renderContext(WindowsEngine::getInstance().getGraphics().getContext())
{
	DirectX::CreateDDSTextureFromFile(m_renderContext.device, m_path.c_str(), nullptr, &m_texture);
}

Texture& Texture::operator=(const Texture& other) {
	Texture{ other }.swap(*this);
	return *this;

}

Texture::Texture(Texture&& other) noexcept
	: m_path(std::exchange(other.m_path, L""))
	#ifdef D3D11_IMPL
	, m_renderContext(other.m_renderContext)
	, m_texture(std::exchange(other.m_texture, nullptr))
	#endif
{	}
Texture& Texture::operator=(Texture&& other) noexcept
{
	Texture{ std::move(other) }.swap(*this);
	return *this;
}


Texture::~Texture()
{
	DX_RELEASE(m_texture);
}

