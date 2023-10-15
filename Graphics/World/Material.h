#pragma once

#include <unordered_map>
#include <string>
#include <directXmath.h>

#include "../abstraction/Texture.h"

using namespace DirectX;

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


/*
ILLUMINATION MODEL
0. Color on and Ambient off
1. Color on and Ambient on
2. Highlight on
3. Reflection on and Ray trace on
4. Transparency: Glass on, Reflection: Ray trace on
5. Reflection: Fresnel on and Ray trace on
6. Transparency: Refraction on, Reflection: Fresnel off and Ray trace on
7. Transparency: Refraction on, Reflection: Fresnel on and Ray trace on
8. Reflection on and Ray trace off
9. Transparency: Glass on, Reflection: Ray trace off
10. Casts shadows onto invisible surfaces
*/

struct MaterialCoefs
{
	float Ka[4];	// ambiant	
	float Kd[4];	// diffuse
	float Ks[4];	// specular
	float Ke[4];	// emissive	
	float Ns;		// exponent specular	
	float Ni; // refraction, optical density	
	float transparency; // 1 - d

	float padding;

};

class Material {


private:

	std::unordered_map<TextureType, Texture> m_textures;
	MaterialCoefs m_coefs;
	int m_illuminationModel;

public:


	void setCoefficients(const MaterialCoefs& coefs)	noexcept { m_coefs = coefs; }
	void setModel(int model)							noexcept { m_illuminationModel = model; }
	

	template<TextureType T>
	[[nodiscard]] const Texture&		queryTexture()	  const noexcept	{ return m_textures.at(T);}
	[[nodiscard]] const MaterialCoefs&	getCoefficients() const noexcept	{ return m_coefs; }

	void loadTextures(const std::initializer_list<std::pair<std::string, TextureType>>& textures)
	{
		for (auto& t : textures)
			m_textures[t.second] = std::move(Texture(widestring2string2(t.first)));
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Material()
		: m_textures(std::unordered_map<TextureType, Texture>{})
		, m_coefs{}
	, m_illuminationModel(0)		
	{	
	}
	
	void swap(Material& other)
	{
		static_assert(sizeof(float[4]) == sizeof(XMVECTOR));
		std::swap(other.m_textures, m_textures);
		std::swap(other.m_coefs, m_coefs);
		std::swap(other.m_illuminationModel, m_illuminationModel);
	}

	Material(const Material& other) 
		: m_textures(other.m_textures)
		, m_coefs(other.m_coefs)
		, m_illuminationModel(other.m_illuminationModel)
	{}

	Material& operator=(const Material& other) 
	{
		Material{ other }.swap(*this);
		return *this;
	}

	Material(Material&& other) noexcept
		: m_textures(std::exchange(other.m_textures, {}))
		, m_coefs(std::exchange(other.m_coefs, {}))
		, m_illuminationModel(std::exchange(other.m_illuminationModel, 0))
	{}

	Material& operator=(Material&& other) noexcept {
		Material{ std::move(other) }.swap(*this);
		return *this;
	}

	~Material() = default;
};