#pragma once

#include <DirectXMath.h>

class Sprite
{
private:

	DirectX::XMVECTOR m_position;
	DirectX::XMVECTOR m_size;
	DirectX::XMVECTOR m_uv;

public:

	consteval Sprite() = default;

	Sprite(const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& size, const DirectX::XMVECTOR& uv)
		: m_position(pos)
		, m_size(size)
		, m_uv(uv)
	{}
	
};
