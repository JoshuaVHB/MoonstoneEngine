#pragma once

#include <wrl/client.h>
#include <filesystem>
#include "../../Platform/WindowsEngine.h"

struct ID3D11ShaderResourceView;

namespace fs = std::filesystem;
class Texture 
{
private:

	std::wstring m_path;

#ifdef D3D11_IMPL
	//ID3D11Texture2D* m_texture = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture = nullptr;
	d3d11_graphics::RenderingContext m_renderContext;
	D3D11_TEXTURE2D_DESC m_desc;
#endif

public:

	Texture(const std::wstring& path);
	Texture(const std::string& path);
	Texture(int width, int height);
	Texture(ID3D11ShaderResourceView*);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// -- Basic operations

	ID3D11ShaderResourceView* getTexture() const { return m_texture.Get(); }

	
	static Texture toTextureArray(
		d3d11_graphics::RenderingContext renderContext,
		const std::vector<Texture>& textures,
		const int width = -1,
		const int height = -1
	);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// -- Getters
	///

	[[nodiscard]] int getWidth() const noexcept { return m_desc.Width; }
	[[nodiscard]] int getHeight() const noexcept { return m_desc.Height; }

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// -- Constructor and stuff

	Texture() = default;
	Texture(nullptr_t) {};
	void swap(Texture& other) noexcept;
	Texture(const Texture& other);
	Texture& operator=(const Texture& other);
	Texture(Texture&& other) noexcept;
	Texture& operator=(Texture&& other) noexcept;
	~Texture();



};