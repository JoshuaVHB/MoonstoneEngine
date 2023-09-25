#pragma once

#include <directXmath.h>
#include <d3d11.h>

#include "../abstraction/IndexBuffer.h"
#include "../abstraction/VertexBuffer.h"

using namespace DirectX;

class Mesh
{
private:

	XMMATRIX m_worldMat = XMMatrixIdentity();

	VertexBuffer m_vbo;
	IndexBuffer m_ibo;

public:

	Mesh() = default;

	void draw() = delete;



};