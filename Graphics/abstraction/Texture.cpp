#include "Texture.h"

#include <filesystem>
#include <utility>
#include <d3d11.h>
#include <locale>
#include <codecvt>

#include "../../Utils/Debug.h"
#include "../../Platform/IO/DDSTextureLoader11.h"
#include "../../Platform/WindowsEngine.h"

inline std::string narrow(const std::wstring& string)
{
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;
	return converter.to_bytes(string);
}


inline std::wstring widen(const std::string& string)
{
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;
	return converter.from_bytes(string);
}


namespace fs = std::filesystem;

Texture Texture::toTextureArray(
	d3d11_graphics::RenderingContext renderContext,
	const std::vector<Texture>& textures,
	const int width/* = -1*/,
	const int height/* = -1*/
)
{
	ID3D11Texture2D* tex;
	int w = width, h = height;
	if (width == -1 || height == -1) {
		w = textures[0].getWidth();
		h = textures[0].getHeight();
	}
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = w;  desc.Height = h;
	desc.MipLevels = desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.ArraySize = static_cast<UINT>(textures.size());



	D3D11_SUBRESOURCE_DATA initData{};

	initData.pSysMem = textures.data();
	initData.SysMemSlicePitch = sizeof(Texture);

	renderContext.device->CreateTexture2D(&desc,&initData, &tex);
	return Texture();
}

Texture::Texture(const std::wstring& path)
: m_path(path), m_texture(nullptr)
{
	if (!std::filesystem::exists(path))
		std::cout << "WARNING : texture" << narrow(path) << " not found. Check for typo !" << std::endl;
#ifdef D3D11_IMPL
	m_renderContext = WindowsEngine::getInstance().getGraphics().getContext();
	DirectX::CreateDDSTextureFromFile(m_renderContext.device, path.c_str(), nullptr, &m_texture);
	//reinterpret_cast<ID3D11Texture2D*>(m_texture.Get())->GetDesc(&m_desc);
#endif
}

Texture::Texture(const std::string& path) : Texture(widen(path))
{}


Texture::Texture(ID3D11ShaderResourceView* srv)
{
	m_renderContext = WindowsEngine::getInstance().getGraphics().getContext();
	m_texture = srv;
}


Texture::Texture(int width, int height)
	: m_texture(nullptr)
{
#ifdef D3D11_IMPL
	m_renderContext = WindowsEngine::getInstance().getGraphics().getContext();

	constexpr static const uint32_t s_pixel = 0xffffffff;

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
	m_desc = desc;
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
	std::swap(other.m_desc, m_desc);
	m_renderContext = WindowsEngine::getInstance().getGraphics().getContext();
#endif
}

Texture::Texture(const Texture& other)
	: m_path(other.m_path)
	, m_renderContext(WindowsEngine::getInstance().getGraphics().getContext())
{
	DirectX::CreateDDSTextureFromFile(m_renderContext.device, m_path.c_str(), nullptr, &m_texture);
	//reinterpret_cast<ID3D11Texture2D*>(m_texture.Get())->GetDesc(&m_desc);
}

Texture& Texture::operator=(const Texture& other) {
	Texture{ other }.swap(*this);
	return *this;

}

Texture::Texture(Texture&& other) noexcept
	: m_path(std::exchange(other.m_path, L""))
	#ifdef D3D11_IMPL
	, m_texture(std::exchange(other.m_texture, nullptr))
	, m_renderContext(other.m_renderContext)
	, m_desc(std::exchange(other.m_desc, {}))
	#endif
{	}
Texture& Texture::operator=(Texture&& other) noexcept
{
	Texture{ std::move(other) }.swap(*this);
	return *this;
}


Texture::~Texture()
{
	//DX_RELEASE(m_texture);
}


