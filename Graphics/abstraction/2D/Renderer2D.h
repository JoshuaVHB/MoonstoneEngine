#pragma once

#include <vector>

// make this an instanced
#define _XM_NO_INTRINSICS_
#include <DirectXMath.h>

#include "abstraction/VertexBuffer.h"
#include "abstraction/IndexBuffer.h"
#include "abstraction/Effect.h"
#include "abstraction/Texture.h"


class Renderer2D {

private:

	Effect m_quadEffect;

	VertexBuffer m_vbo;
	IndexBuffer m_ibo;

	std::vector<SpriteVertex> m_batchedVertices;
	std::vector<IndexBuffer::size_type> m_batchedIndices;

	std::vector<Texture> m_textureArray;
	Texture m_whitePixel{1,1};

public:

	Renderer2D()
	{
		m_quadEffect.loadEffectFromFile("res/effects/quad.fx");

		InputLayout layout;
		layout.pushBack<2>(InputLayout::Semantic::Position);
		layout.pushBack<4>(InputLayout::Semantic::Texcoord);
		layout.pushBack<4>(InputLayout::Semantic::Color);

		m_quadEffect.bindInputLayout(layout);
	}


	void renderQuad();
	void renderQuadTextured();
	void renderQuadColored();

	////////////////////////////////////////////////////////////
	///
	
	void batchQuad();
	void renderBatched();

	size_t getInstanceCount();
	size_t getVertexCount();



};