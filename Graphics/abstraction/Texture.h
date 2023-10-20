#pragma once

#include <filesystem>
#include "../../Platform/WindowsEngine.h"

struct ID3D11ShaderResourceView;

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

	Texture(const std::wstring& path);
	Texture(int width, int height);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// -- Basic operations

	ID3D11ShaderResourceView* getTexture() const { return m_texture; }


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// -- Constructor and stuff

	Texture() = default;
	void swap(Texture& other) noexcept;
	Texture(const Texture& other);
	Texture& operator=(const Texture& other);
	Texture(Texture&& other) noexcept;
	Texture& operator=(Texture&& other) noexcept;
	~Texture();



};