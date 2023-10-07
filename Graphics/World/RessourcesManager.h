#pragma once


#include <vector>

#include "../abstraction/Texture.h"

struct TextureManager {

	std::vector<Texture*> textures;

	Texture* importTexture(const std::wstring& path) {}
	Texture* getTextureFromName(const std::wstring& name) {}

};



struct RessourcesManager {

	TextureManager textureManager;

} s_RessourcesManager ;