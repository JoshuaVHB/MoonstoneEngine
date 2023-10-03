#pragma once

#include <filesystem>
#include <d3d11.h>


namespace fs = std::filesystem;
class Texture 

{

private:

	ID3D11ShaderResourceView* m_Texture;


public:

	Texture() {}

	Texture(const std::wstring& path) {}




};