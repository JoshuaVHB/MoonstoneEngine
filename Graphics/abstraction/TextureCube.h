#pragma once

#include <filesystem>
#include <d3d11.h>
#include <array>

#include "../../Utils/Debug.h"
#include "../../Platform/IO/DDSTextureLoader11.h"
#include "../../Platform/WindowsEngine.h"



#include <locale>
#include <codecvt>

#define SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING 1

inline std::wstring widestring2string(const std::string& string)
{
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;
	return converter.from_bytes(string);
}
namespace fs = std::filesystem;

class TextureCube {

private:

	std::string m_path;

#ifdef D3D11_IMPL
	ID3D11ShaderResourceView* m_srv;
	ID3D11Resource* resource;
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

		if (DirectX::CreateDDSTextureFromFileEx(
			m_renderContext.device,
			m_renderContext.context,
			widestring2string(path).c_str(),
			0,
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_SHADER_RESOURCE,
			0,
			D3D11_RESOURCE_MISC_TEXTURECUBE,
			DirectX::DDS_LOADER_DEFAULT,
			&resource,
			&m_srv
		)!=S_OK) throw;

#endif
	}

	ID3D11ShaderResourceView* getResourceView() const { return m_srv; }

};