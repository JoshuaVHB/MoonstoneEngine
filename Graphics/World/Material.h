#pragma once

#include <unordered_map>
#include <string>

#include "../abstraction/Texture.h"



enum class TextureType {
	ALBEDO,
	NORMAL,
	BUMP,
	HEIGHT,
	ROUGHNESS,
	METALNESS,
	SPECULAR,
};



#include <locale>
#include <codecvt>
inline std::wstring widestring2string2(const std::string& string)
{
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;
	return converter.from_bytes(string);
}


class Material {


private:

	using TexInfos = std::pair<TextureType, Texture>;

	std::unordered_map<TextureType, Texture> m_textures;

public:


	template<TextureType T>
	const Texture& queryTexture() const {
		return m_textures.at(T);
	}


	void loadTextures(std::initializer_list < std::pair<std::string, TextureType >> textures)
	{
		for (auto& t : textures)
			m_textures[t.second] = std::move(Texture(widestring2string2(t.first)));
	}
	

};