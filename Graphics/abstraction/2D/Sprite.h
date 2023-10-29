#pragma once


#include <DirectXMath.h>
#include "../Texture.h"
#include "../Effect.h"
#include "../VertexBuffer.h"
#include "../../../Platform/WindowsEngine.h"
#include "Font.h"

// Highly experimental

class Sprite
{
private:

	DirectX::XMVECTOR m_position;
	DirectX::XMVECTOR m_size;
	DirectX::XMVECTOR m_uv;

	Texture m_texture;

public:

	Sprite() = default;

	Sprite(const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& size, const DirectX::XMVECTOR& uv)
		: m_position(pos)
		, m_size(size)
		, m_uv(uv)
	{}
	
};

