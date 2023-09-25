#pragma once

#include <concepts>
#include <d3d11.h>
#include <vector>
#include <cstdint>


#include "Vertex.h"

#include "../../Utils/Debug.h"




class VertexBuffer
{
	std::vector<Vertex> m_vertices;

	// -- Object creation
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;

	// -- Buffer stuff

	D3D11_BUFFER_DESC m_descriptor;
	ID3D11Buffer* m_vbo;
	D3D11_SUBRESOURCE_DATA m_initData;

public:

	VertexBuffer() {}

	VertexBuffer(ID3D11Device* device,
		ID3D11DeviceContext* context,
		const std::vector<Vertex>& vertices)
	{
		m_vertices = vertices;
		m_device = device;
		m_context = context;
		// -- Vertex buffer
		ZeroMemory(&m_descriptor, sizeof(m_descriptor));

		m_descriptor.Usage = D3D11_USAGE_IMMUTABLE;
		m_descriptor.ByteWidth = sizeof(Vertex)*vertices.size();
		m_descriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		m_descriptor.CPUAccessFlags = 0;

		ZeroMemory(&m_initData, sizeof(m_initData));
		m_initData.pSysMem = &vertices[0];

		DX_TRY(device->CreateBuffer(&m_descriptor, &m_initData, &m_vbo));

	}

	void bind()
	{
		const UINT stride = sizeof(Vertex);
		const UINT offset = 0;
		m_context->IASetVertexBuffers(0, 1, &m_vbo, &stride, &offset);
	}




};

