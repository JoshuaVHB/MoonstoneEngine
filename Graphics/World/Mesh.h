#pragma once

#include <directXmath.h>
#include <d3d11.h>
#include <vector>

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



};