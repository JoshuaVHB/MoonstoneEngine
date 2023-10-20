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

Texture::Texture(int width, int height)
	: m_texture(nullptr)
{
#ifdef D3D11_IMPL
	m_renderContext = WindowsEngine::getInstance().getGraphics().getContext();

	constexpr static const uint32_t s_pixel = 0xffc99aff;

	D3D11_SUBRESOURCE_DATA initData = { &s_pixel, sizeof(uint32_t), 0 };

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = width;  desc.Height = height;
	desc.MipLevels = desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	ID3D11Texture2D* tex;
	HRESULT hr = m_renderContext.device->CreateTexture2D(&desc, &initData, &tex);

	if (SUCCEEDED(hr))
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;
		hr = m_renderContext.device->CreateShaderResourceView(tex, &SRVDesc, &m_texture);
	}



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


