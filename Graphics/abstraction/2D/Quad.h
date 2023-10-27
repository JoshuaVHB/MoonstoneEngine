#pragma once

#include "../VertexBuffer.h"
#include "../IndexBuffer.h"
#include "../Camera.h"

#include "../../Renderer.h"


#include <DirectXMath.h>
using namespace DirectX;


class Quad
{
private:

	const VertexBuffer m_vbo = 
	{
		{
			
		Vertex{ XMVECTOR{0,0}},
		Vertex{ XMVECTOR{1,0}},
		Vertex{ XMVECTOR{1,1}},
		Vertex{ XMVECTOR{0,1}},
		}
	};
	const IndexBuffer m_ibo= IndexBuffer{
		{
			0,1,2,
			2,3,0
		}};

	XMVECTOR m_screenPos;
	


public:



	void drawQuad(Camera& cam)
	{
		m_vbo.bind();
		m_ibo.bind();
		
	}

};
