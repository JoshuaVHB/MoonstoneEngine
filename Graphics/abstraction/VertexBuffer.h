#pragma once

#include <concepts>
#include <d3d11.h>
#include <vector>
#include <cstdint>
#include <algorithm>


#include "Vertex.h"

#include "../../Utils/Debug.h"
#include "../../Platform/WindowsEngine.h"



class VertexBuffer
{
	std::vector<Vertex> m_vertices;

	// -- Buffer stuff
	ID3D11Buffer* m_vbo				= nullptr;


#ifdef D3D11_IMPL
	d3d11_graphics::RenderingContext m_renderContext;
#endif

public:

	VertexBuffer() {}

	VertexBuffer(const std::vector<Vertex>& vertices)
	{
#ifdef D3D11_IMPL
		m_renderContext = WindowsEngine::getInstance().getGraphics().getContext();
		m_vertices = vertices;

		D3D11_BUFFER_DESC m_descriptor{};
		D3D11_SUBRESOURCE_DATA m_initData{};
		// -- Vertex buffer
		ZeroMemory(&m_descriptor, sizeof(m_descriptor));

		m_descriptor.Usage = D3D11_USAGE_IMMUTABLE;
		m_descriptor.ByteWidth = static_cast<UINT>(vertices.size()) * sizeof(Vertex);
		m_descriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		m_descriptor.CPUAccessFlags = 0;

		ZeroMemory(&m_initData, sizeof(m_initData));
		m_initData.pSysMem = &vertices[0];

		DX_TRY(m_renderContext.device->CreateBuffer(&m_descriptor, &m_initData, &m_vbo));
#endif
	}

	void bind() const
	{
		const UINT stride = sizeof(Vertex);
		const UINT offset = 0;

#ifdef D3D11_IMPL
		m_renderContext.context->IASetVertexBuffers(0, 1, &m_vbo, &stride, &offset);
#endif
	}

	void swap(VertexBuffer& other) {
		std::swap(other.m_vbo, m_vbo);
		std::swap(other.m_vertices, m_vertices);
		std::swap(other.m_renderContext, m_renderContext);
	}

	VertexBuffer(const VertexBuffer&) = delete;
	VertexBuffer& operator=(const VertexBuffer&) = delete;

	VertexBuffer(VertexBuffer&& other) noexcept
		: m_renderContext(other.m_renderContext)
		, m_vbo(std::exchange(other.m_vbo, nullptr))
		, m_vertices(std::exchange(other.m_vertices, {}))
	{	}
	VertexBuffer& operator=(VertexBuffer&& other) noexcept
	{
		VertexBuffer{ std::move(other) }.swap(*this);
		return *this;
	}

	~VertexBuffer()
	{
		DX_RELEASE(m_vbo);
	}




};

