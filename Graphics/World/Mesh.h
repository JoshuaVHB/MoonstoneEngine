#pragma once

#include <directXmath.h>
#include <d3d11.h>
#include <vector>
#include <utility>
#include <algorithm>

#include "../Renderer.h"
#include "../abstraction/IndexBuffer.h"
#include "../abstraction/VertexBuffer.h"

using namespace DirectX;

class Mesh
{
private:

	VertexBuffer m_vbo;
	IndexBuffer m_ibo;


public:
	
	XMMATRIX m_worldMat = XMMatrixIdentity();


	Mesh() = default;

	Mesh(std::vector<Vertex> vertices, std::vector<uint16_t> indices) 
	{
		m_vbo = VertexBuffer(vertices);
		m_ibo = IndexBuffer(indices);
	};

	void draw() const {

		m_vbo.bind();
		m_ibo.bind();
		Renderer::drawIndexed(m_ibo.getBufferSize(), 0, 0);
	}

	void swap(Mesh& other)
	{
		std::swap(other.m_worldMat, m_worldMat);
		std::swap(other.m_ibo, m_ibo);
		std::swap(other.m_vbo, m_vbo);
	}

	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;


	Mesh(Mesh&& other) noexcept
		: m_ibo(std::exchange(other.m_ibo, {}))
		, m_worldMat(std::exchange(other.m_worldMat, {}))
		, m_vbo(std::exchange(other.m_vbo,{}))
	{
	}

	Mesh& operator=(Mesh&& other) noexcept {
		Mesh{ std::move(other) }.swap(*this);
		return *this;
	}



};