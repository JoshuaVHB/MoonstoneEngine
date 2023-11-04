#pragma once

#include <vector>
#include <map>
#include <unordered_map>

// make this an instanced
#define _XM_NO_INTRINSICS_
#include <DirectXMath.h>

#include "abstraction/VertexBuffer.h"
#include "abstraction/IndexBuffer.h"
#include "abstraction/Effect.h"
#include "abstraction/Texture.h"


struct Quad 
{
	XMVECTOR position; // (x,y,size_x,size_y)
	XMVECTOR size;
	XMVECTOR uv_v4; // (u,v,size_u,size_v)
	Texture* texture = nullptr;
	XMVECTOR color = { 0,0,0,1 };
};

class Renderer2D {

private:

	Effect m_quadEffect;

	VertexBuffer m_vbo;
	IndexBuffer m_ibo;

	std::vector<TexturedSpriteVertex> m_batchedVertices;
	std::vector<IndexBuffer::size_type> m_batchedIndices;

	std::vector<Texture> m_textureArray;
	std::unordered_map<Texture*, int> m_textureMap;

	Texture m_whitePixel{1,1};
	int currentSlot = 0;
public:

	Renderer2D()
	{
		m_quadEffect.loadEffectFromFile("res/effects/quad.fx");

		InputLayout layout;
		layout.pushBack<4>(InputLayout::Semantic::Position);
		layout.pushBack<4>(InputLayout::Semantic::Texcoord);
		layout.pushBack<4>("TEXID");

		m_quadEffect.bindInputLayout(layout);
	}


	void renderQuad();
	void renderQuadTextured();
	void renderQuadColored();

	////////////////////////////////////////////////////////////
	///
	
	void batchQuad(const Quad& quad)
	{
		float texSlot = 0;
		if (m_textureMap.contains(quad.texture))
		{
			texSlot = m_textureMap[quad.texture];
		}
		else {
			m_textureMap[quad.texture] = currentSlot;
			currentSlot++;

		}


		m_batchedVertices.push_back(TexturedSpriteVertex{
			// (x,y,size_x,size_y)
			{XMVectorGetX(quad.position),	XMVectorGetY(quad.position),	XMVectorGetX(quad.size),	XMVectorGetY(quad.size)},
			// (u,v,size_u,size_v)
			quad.uv_v4,
			// texid
			{texSlot,texSlot,texSlot,texSlot}
			});
	}


	void renderBatched()
	{
		std::vector<ID3D11ShaderResourceView*> srvs;
		srvs.reserve(8);		

		//m_quadEffect.bindTextureArray("tex_slots", )
		//m_quadEffect.apply();


	}

	size_t getInstanceCount();
	size_t getVertexCount();



};