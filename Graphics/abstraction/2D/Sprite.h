#pragma once


#include <DirectXMath.h>
#include "../Texture.h"
#include "../Effect.h"
#include "../VertexBuffer.h"
#include "../../../Platform/WindowsEngine.h"

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


class InstancedSprite {

private:

	ID3D11Buffer* m_instancedBuffer;
	ID3D11Buffer* m_vertexBuffer;

	d3d11_graphics::RenderingContext m_renderContext;

	static const size_t m_instanceCount = 4;
	using InstanceType = SpriteVertex;
	InputLayout m_layout;
	Effect m_spriteEffect;

public:

	InstancedSprite() {
		m_renderContext = WindowsEngine::getInstance().getGraphics().getContext();

		{

			SpriteVertex vertices[6]
			{
				{{0,0},{0,0}},
				{{1,0},{1,0}},
				{{1,1},{1,1}},
				{{1,1},{1,1}},
				{{0,1},{0,1}},
				{{0,0},{0,0}},
			};

			D3D11_BUFFER_DESC descriptor{};
			D3D11_SUBRESOURCE_DATA m_initData{};
			// -- Vertex buffer

			descriptor.Usage = D3D11_USAGE_IMMUTABLE;
			descriptor.ByteWidth = static_cast<UINT>(6) * sizeof(SpriteVertex);
			descriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			descriptor.CPUAccessFlags = 0;
			m_initData.pSysMem = &vertices[0];
			m_renderContext.device->CreateBuffer(&descriptor, &m_initData, &m_vertexBuffer);
		}

		/////
		{

			InstanceType instances[m_instanceCount];

			instances[0].position = {-1,-1,0,0 };
			instances[1].position = {0,-1,0,0 };
			instances[3].position = {0,0,0,0 };
			instances[2].position = {-1,0,0,0 };


			instances[0].uv = {1,0};
			instances[1].uv = {1,0};
			instances[2].uv = {1,0};
			instances[3].uv = {1,0};

			D3D11_BUFFER_DESC desc_instance{};
			D3D11_SUBRESOURCE_DATA initInstance{};
			// -- Vertex buffer

			desc_instance.Usage = D3D11_USAGE_DEFAULT;
			desc_instance.ByteWidth = static_cast<UINT>(4) * sizeof(InstanceType);
			desc_instance.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc_instance.CPUAccessFlags = 0;
			desc_instance.StructureByteStride = 0;
			desc_instance.MiscFlags = 0;
			initInstance.pSysMem = &instances[0];
			m_renderContext.device->CreateBuffer(&desc_instance, &initInstance, &m_instancedBuffer);
		}


		m_layout.pushBack<3>(InputLayout::Semantic::Position);
		m_layout.pushBack<2>(InputLayout::Semantic::Texcoord);

		m_layout.pushBackInstanced<3>(InputLayout::Semantic::Texcoord, 1);	// instance pos
		m_layout.pushBackInstanced<2>(InputLayout::Semantic::Texcoord, 2); // instance uv


		m_spriteEffect.loadEffectFromFile("res/effects/sprite2D.fx");

		m_spriteEffect.bindInputLayout(m_layout);

	}

	void bind()
	{
		unsigned int strides[2];
		unsigned int offsets[2];
		ID3D11Buffer* bufferPointers[2];

		strides[0] = sizeof(SpriteVertex);
		strides[1] = sizeof(InstanceType);

		offsets[0] = 0;
		offsets[1] = 0;
	
		bufferPointers[0] = m_vertexBuffer;
		bufferPointers[1] = m_instancedBuffer;
	
		m_renderContext.context->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);

	}


	void render() {

		m_renderContext.context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_renderContext.context->IASetInputLayout(m_spriteEffect.getVertexLayout());
		m_spriteEffect.apply();

		m_renderContext.context->DrawInstanced(6 , m_instanceCount, 0, 0);

	}

};