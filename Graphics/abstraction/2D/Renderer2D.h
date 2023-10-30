#pragma once

// make this an instanced
#define _XM_NO_INTRINSICS_
#include <DirectXMath.h>

#include "abstraction/VertexBuffer.h"
#include "abstraction/IndexBuffer.h"
#include "abstraction/Effect.h"


class Renderer2D {

private:

	Effect m_spriteEffect;

public:

	Renderer2D()
	{
		m_spriteEffect.loadEffectFromFile("res/effects/sprite2D.fx");
	}


	size_t getInstanceCount();
	size_t getVertexCount();



};