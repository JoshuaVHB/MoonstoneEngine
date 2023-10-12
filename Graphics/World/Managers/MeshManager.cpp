#include <rapidobj.hpp>

#include "MeshManager.h"
#include "../../abstraction/Vertex.h"
#include "../Mesh.h"

#include <iostream>
#include <vector>
#include <directXmath.h>


void ReportError(const rapidobj::Error& error)
{
	std::cout << error.code.message() << "\n";
	if (!error.line.empty()) {
		std::cout << "On line " << error.line_num << ": \"" << error.line << "\"\n";
	}
}

Mesh MeshManager::loadMeshFromFile(const fs::path& pathToFile) {

	bool pls = std::filesystem::exists(pathToFile);

	rapidobj::MaterialLibrary mtl_library = rapidobj::MaterialLibrary::Default();
	rapidobj::Result res = rapidobj::ParseFile(pathToFile);


	rapidobj::Triangulate(res);



	// -- Generate the mesh

	std::vector<Vertex> vertices;
	std::vector<uint16_t> indices;

	size_t num_triangles{};


	for (const auto& shape : res.shapes) {
		num_triangles += shape.mesh.num_face_vertices.size();
	}
	/*
	for (size_t i = 0; i < num_triangles; i++)
	{
		DirectX::XMVECTOR pos = {
			res.attributes.positions[i * 3],
			res.attributes.positions[i * 3 + 1],
			res.attributes.positions[i * 3 + 2]
		};

		DirectX::XMVECTOR normals = {
			res.attributes.normals[i * 3],
			res.attributes.normals[i * 3 + 1],
			res.attributes.normals[i * 3 + 2]
		};

	}
	*/

	return Mesh(vertices, indices);
}