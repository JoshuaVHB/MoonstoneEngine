#pragma once

#include <vector>

#include "../abstraction/IndexBuffer.h"
#include "../abstraction/VertexBuffer.h"

#include "../../Utils/Transform.h"
#include "../../Utils/AABB.h"
#include "Material.h"	

using namespace DirectX;

//class Material;

class Mesh
{
private:

	VertexBuffer m_vbo;
	IndexBuffer m_ibo;

	std::vector<Material> m_materials;

	std::vector<IndexBuffer::size_type> m_submeshes;
	std::vector<uint16_t> m_submeshMats;

	Transform m_transform;
	AABB m_boundingBox{};

public:
	

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Mesh() = default;

	Mesh(const std::vector<Vertex>& vertices, const std::vector<IndexBuffer::size_type>& indices);

	Mesh(const std::vector<Vertex>& vertices,
		const std::vector<IndexBuffer::size_type>& indices,
		const std::vector<IndexBuffer::size_type>& submeshIndices);

	Mesh(const std::vector<Vertex>& vertices, const std::vector<IndexBuffer::size_type>& indices,
		const std::vector<IndexBuffer::size_type>& submeshIndices,
		const std::vector<Material>& materials,
		const std::vector<uint16_t>& submeshMats);


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	[[nodiscard]] const std::vector<Material>&			getMaterials()			const noexcept	{ return m_materials;	}
	[[nodiscard]] std::vector<uint16_t>					getSubmeshMaterials()	const noexcept	{ return m_submeshMats; }
	[[nodiscard]] std::vector<IndexBuffer::size_type>	getSubmeshIndices()		const noexcept	{ return m_submeshes;	}
	[[nodiscard]] Transform&							getTransform()				  noexcept	{ return m_transform;	}
	[[nodiscard]] const Transform&						getTransform()			const noexcept	{ return m_transform;	}
	[[nodiscard]] AABB&									getBoundingBox()			  noexcept	{ return m_boundingBox;	}
	[[nodiscard]] const AABB&							getBoundingBox()		const noexcept	{ return m_boundingBox;	}


	[[nodiscard]] size_t	getVerticesCount()			const  noexcept	{ return m_vbo.getVerticesCount(); }
	[[nodiscard]] size_t	getTriangleCount()			const  noexcept	{ return m_ibo.getIndicesCount()/3u; }
	[[nodiscard]] size_t	getIndicesCount()			const  noexcept	{ return m_ibo.getIndicesCount(); }
		 
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void draw() const;

	// Very inaccurate
	void computeBoundingBox() noexcept;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void swap(Mesh& other) noexcept;
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;
	Mesh(Mesh&& other) noexcept;
	Mesh& operator=(Mesh&& other) noexcept;
	~Mesh()=default;

};