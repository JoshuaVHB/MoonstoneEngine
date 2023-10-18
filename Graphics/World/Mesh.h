#pragma once

#include <directXmath.h>
#include <d3d11.h>
#include <vector>
#include <utility>
#include <algorithm>

#include "../Renderer.h"
#include "../abstraction/IndexBuffer.h"
#include "../abstraction/VertexBuffer.h"
#include "../World/Material.h"

#include "../../Utils/Transform.h"

using namespace DirectX;

class Mesh
{
private:

	VertexBuffer m_vbo;
	IndexBuffer m_ibo;

	std::vector<Material> m_materials;

	std::vector<IndexBuffer::size_type> m_submeshes;
	std::vector<uint16_t> m_submeshMats;

	Transform m_transform;

public:
	

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Mesh() = default;

	Mesh(std::vector<Vertex> vertices, std::vector<IndexBuffer::size_type> indices) 
	{
		m_vbo = VertexBuffer(vertices);
		m_ibo = IndexBuffer(indices);
		m_submeshes = std::vector<IndexBuffer::size_type>{ 0, (IndexBuffer::size_type)(indices.size())};
	};

	Mesh(std::vector<Vertex> vertices, std::vector<IndexBuffer::size_type> indices, const std::vector<IndexBuffer::size_type>& submeshIndices)
	{
		m_vbo = VertexBuffer(vertices);
		m_ibo = IndexBuffer(indices);
		m_submeshes = submeshIndices;
		m_materials = {};
	};

	Mesh(const std::vector<Vertex>& vertices, const std::vector<IndexBuffer::size_type>& indices,
		const std::vector<IndexBuffer::size_type>& submeshIndices,
		const std::vector<Material>& materials,
		const std::vector<uint16_t>& submeshMats
	)
	{
		m_vbo = VertexBuffer(vertices);
		m_ibo = IndexBuffer(indices);
		m_submeshes = submeshIndices;
		m_materials = materials;
		m_submeshMats = submeshMats;
	};


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	[[nodiscard]] const std::vector<Material>& getMaterials()					const noexcept	{ return m_materials;	}
	[[nodiscard]] std::vector<uint16_t> getSubmeshMaterials()					const noexcept	{ return m_submeshMats; }
	[[nodiscard]] std::vector<IndexBuffer::size_type> getSubmeshIndices()		const noexcept	{ return m_submeshes;	}
	[[nodiscard]] Transform&							getTransform()				  noexcept	{ return m_transform;	}
	[[nodiscard]] const Transform&						getTransform()			const  noexcept	{ return m_transform;	}


	[[nodiscard]] int	getVerticesCount()			const  noexcept	{ return m_vbo.getVerticesCount(); }
	[[nodiscard]] int	getTriangleCount()			const  noexcept	{ return m_ibo.getIndicesCount()/3u; }
	[[nodiscard]] int	getIndicesCount()			const  noexcept	{ return m_ibo.getIndicesCount(); }
		 
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void draw() const {

		m_vbo.bind();
		m_ibo.bind();

		for (int index = 0; index < m_submeshes.size()-1; ++index)
		{
			size_t indexCount = m_submeshes[index + 1] - m_submeshes[index];
			Renderer::drawIndexed(indexCount, m_submeshes[index], 0);
		}

	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void swap(Mesh& other)
	{
		std::swap(other.m_transform, m_transform);
		std::swap(other.m_ibo, m_ibo);
		std::swap(other.m_vbo, m_vbo);
		std::swap(other.m_materials, m_materials);
		std::swap(other.m_submeshMats, m_submeshMats);
		std::swap(other.m_submeshes, m_submeshes);
	}

	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;

	Mesh(Mesh&& other) noexcept
		: m_ibo			(std::exchange(other.m_ibo,			{}))
		, m_transform	(std::exchange(other.m_transform,	{}))
		, m_vbo			(std::exchange(other.m_vbo,			{}))
		, m_materials	(std::exchange(other.m_materials,	std::vector<Material>()))
		, m_submeshMats	(std::exchange(other.m_submeshMats,	std::vector<uint16_t>()))
		, m_submeshes	(std::exchange(other.m_submeshes,	std::vector<IndexBuffer::size_type>()))
	{}

	Mesh& operator=(Mesh&& other) noexcept {
		Mesh{ std::move(other) }.swap(*this);
		return *this;
	}

};