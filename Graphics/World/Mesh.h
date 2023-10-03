#pragma once

#include <directXmath.h>
#include <d3d11.h>
#include <vector>

#include "../abstraction/IndexBuffer.h"
#include "../abstraction/VertexBuffer.h"

using namespace DirectX;

class Mesh
{
private:



	VertexBuffer m_vbo;
	IndexBuffer m_ibo;


	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;

public:
	
	XMMATRIX m_worldMat = XMMatrixIdentity();


	Mesh() = default;

	Mesh(
		ID3D11Device* device,
		ID3D11DeviceContext* context,
		std::vector<Vertex> vertices, std::vector<uint16_t> indices
	) 
	{
		m_device = device;
		m_context = context;

		m_vbo = VertexBuffer(device, context, vertices);
		m_ibo = IndexBuffer(device, context, indices);

	};

	void draw() {

		m_vbo.bind();
		m_ibo.bind();
		m_context->DrawIndexed(static_cast<UINT>(m_ibo.getBufferSize()), 0, 0);
	}



};