#pragma once

#include <vector>
#include <map>
#include <array>
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


	static const size_t MaxQuadCount = 1000;
	static const size_t MaxVertexCount = MaxQuadCount * 4;
	static const size_t MaxIndicesCount = MaxQuadCount * 6;
	static const size_t MaxTextures = 32;


	ID3D11Buffer* m_vbo = nullptr;

	IndexBuffer m_ibo;


	std::vector<TexturedSpriteVertex> m_batchedVertices;
	std::vector<IndexBuffer::size_type> m_batchedIndices;

	std::vector<Texture*> m_textureArray;
	std::unordered_map<const Texture*, int> m_textureMap;
	std::array<const Texture*, 8> m_textures;

	Texture m_whitePixel{1,1};
	int currentSlot = 0;
	bool isBatching = false;

	unsigned int IndexCount = 0;

	std::array<unsigned int, 8> TextureSlots;

	float TextureSlotIndex = 1;



public:

	Renderer2D()
	{
		m_quadEffect.loadEffectFromFile("res/effects/quad.fx");

		InputLayout layout;
		layout.pushBack<4>(InputLayout::Semantic::Position); // (x,y,texId,w)
		layout.pushBack<2>(InputLayout::Semantic::Texcoord);

		m_quadEffect.bindInputLayout(layout);

		m_batchedIndices.resize(MaxIndicesCount);
		int offset = 0;

		for (int i = 0; i < MaxIndicesCount; i += 6) {

			m_batchedIndices[i + 0] = 0 + offset;
			m_batchedIndices[i + 1] = 1 + offset;
			m_batchedIndices[i + 2] = 2 + offset;

			m_batchedIndices[i + 3] = 2 + offset;
			m_batchedIndices[i + 4] = 3 + offset;
			m_batchedIndices[i + 5] = 0 + offset;

			offset += 4;
		}
		m_ibo = IndexBuffer(m_batchedIndices);

	}

	void clear()
	{
		m_batchedVertices.clear();
		m_batchedIndices.clear();
		m_textureMap.clear();
	}


	void renderQuad();
	void renderQuadTextured();
	void renderQuadColored();

	////////////////////////////////////////////////////////////
	///
	
	void batchQuad(XMVECTOR position, XMVECTOR size, const Texture& texture, XMVECTOR uvoffset = {0,0,0,0})
	{

		const auto winSize = WindowsEngine::getInstance().getGraphics().getWinSize();
		if (m_textureMap.size() >= 7 )
		{
			EndBatch();
			Flush();
			m_textureMap.clear();
		}

		if (!m_textureMap.contains(&texture))
		{
			m_textureMap[&texture] = static_cast<int>(m_textureMap.size());
			m_textures[m_textureMap.size() - 1] = &texture;
			
		}


		float quadTexId = static_cast<float>(m_textureMap[&texture]);

		TexturedSpriteVertex quadVertices[4] = {};
		XMVECTOR positions[4] =
		{
				{ (XMVectorGetX(position))/ winSize.first * 2 - 1
						, -2 * (XMVectorGetY(position) / winSize.second) +1
						, quadTexId },


			{		(XMVectorGetX(position) + XMVectorGetX(size)) / winSize.first * 2 - 1,
						-2 * (XMVectorGetY(position) / winSize.second) + 1,
						quadTexId },

			{ (XMVectorGetX(position) + XMVectorGetX(size)) / winSize.first * 2 - 1,
				-2 * ((XMVectorGetY(position) + XMVectorGetY(size)) / winSize.second ) +1,
				quadTexId },

			{ (XMVectorGetX(position)) / winSize.first * 2 - 1,
				-2 * ((XMVectorGetY(position) + XMVectorGetY(size)) / winSize.second)+1 ,
				quadTexId }
		};

		XMVECTOR uv[4] =
		{
			{0,0},
				{1,0},
				{1,1},
				{0,1}
		};


		m_batchedVertices.push_back(TexturedSpriteVertex{positions[0], uv[0]});
		m_batchedVertices.push_back(TexturedSpriteVertex{positions[1], uv[1]});
		m_batchedVertices.push_back(TexturedSpriteVertex{positions[2], uv[2]});
		m_batchedVertices.push_back(TexturedSpriteVertex{positions[3], uv[3]});


	}


	void renderBatched()
	{
		std::vector<ID3D11ShaderResourceView*> srvs;
		srvs.reserve(8);		
	}


	void BeginBatch() {

		isBatching = true;

	}

	void EndBatch() {

		if (m_batchedVertices.empty()) return;
		DX_RELEASE(m_vbo);
		D3D11_BUFFER_DESC descriptor{};
		D3D11_SUBRESOURCE_DATA m_initData{};
		// -- Vertex buffer
		ZeroMemory(&descriptor, sizeof(descriptor));

		descriptor.Usage = D3D11_USAGE_IMMUTABLE;
		descriptor.ByteWidth = static_cast<UINT>(m_batchedVertices.size()) * sizeof(TexturedSpriteVertex);
		descriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		descriptor.CPUAccessFlags = 0;

		ZeroMemory(&m_initData, sizeof(m_initData));
		m_initData.pSysMem = m_batchedVertices.data();
			
		WindowsEngine::getInstance().getGraphics().getContext().device->CreateBuffer(&descriptor, &m_initData, &m_vbo);


		
	}
	void Flush() {

		if (m_batchedVertices.empty()) return;
		bindTextures();
		m_quadEffect.apply();
		// bind textures srv

		constexpr UINT stride = sizeof(TexturedSpriteVertex);
		constexpr UINT offset = 0;

		WindowsEngine::getInstance().getGraphics().getContext().context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		WindowsEngine::getInstance().getGraphics().getContext().context->IASetInputLayout(m_quadEffect.getVertexLayout());
		WindowsEngine::getInstance().getGraphics().getContext().context->IASetVertexBuffers(0, 1, &m_vbo, &stride, &offset);

		m_ibo.bind();



		WindowsEngine::getInstance().getGraphics().getContext().context->DrawIndexed(static_cast<UINT>(m_batchedVertices.size() * 1.5f), 0, 0);


		m_batchedVertices.clear();
		// bind ibo, vbo
		// draw
		// indexcount = 0, texslot = 1
		//unbind


	}

	void bindTextures()
	{
		std::vector<ID3D11ShaderResourceView*> srvs;
		srvs.resize(8);
		for (int i = 0; i < 8; ++i)
		{
			srvs[i] = nullptr;
			if (m_textures[i])
			{
				srvs[i] = m_textures[i]->getTexture();
			}

		}
		m_quadEffect.bindTextureArray("tex_slots", srvs);
	}


	size_t getInstanceCount();
	size_t getVertexCount();



};