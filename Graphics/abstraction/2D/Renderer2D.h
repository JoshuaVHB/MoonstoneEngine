#pragma once

// make this an instanced

#include <DirectXMath.h>

#include "abstraction/VertexBuffer.h"
#include "abstraction/IndexBuffer.h"
#include "abstraction/Effect.h"

struct SpriteVertex
{
	DirectX::XMVECTOR pos;
	DirectX::XMVECTOR uv;
};

class Renderer2D {


};