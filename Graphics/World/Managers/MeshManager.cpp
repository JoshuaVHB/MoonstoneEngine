#include <rapidobj.hpp>

#include "MeshManager.h"
#include "../../abstraction/Vertex.h"
#include "../Mesh.h"

#include <iostream>
#include <vector>
#include <directXmath.h>
#include <tuple>
#include <set>
#include <unordered_set>


void ReportError(const rapidobj::Error& error)
{
	std::cout << error.code.message() << "\n";
	if (!error.line.empty()) {
		std::cout << "On line " << error.line_num << ": \"" << error.line << "\"\n";
	}
}

void assertValid(const rapidobj::Result& res) {
	if (res.error)
	{
		ReportError(res.error);
		throw;
	}
}


Vertex generateVertexFromKey(const rapidobj::Index& key, const rapidobj::Result& loadedResult)
{
	return
	{

		{
			loadedResult.attributes.positions[key.position_index * 3],
			loadedResult.attributes.positions[key.position_index * 3 + 1],
			loadedResult.attributes.positions[key.position_index * 3 + 2],
		},

		{
			loadedResult.attributes.normals[key.normal_index * 3],
			loadedResult.attributes.normals[key.normal_index * 3 + 1],
			loadedResult.attributes.normals[key.normal_index * 3 + 2],
		},

		{
			loadedResult.attributes.texcoords[key.texcoord_index * 2],
			loadedResult.attributes.texcoords[key.texcoord_index * 2 + 1]
		},
	};
}


Mesh MeshManager::loadMeshFromFile(const fs::path& pathToFile) {

	bool pls = std::filesystem::exists(pathToFile);

	rapidobj::MaterialLibrary mtl_library = rapidobj::MaterialLibrary::Default();
	rapidobj::Result res = rapidobj::ParseFile(pathToFile);

	assertValid(res);
	rapidobj::Triangulate(res);
	assertValid(res);


	// -- Generate the mesh

	using key = std::tuple<int, int, int>;
	std::set<key> cachedKeys; // todo use a haser for the rapidobj::Index

	std::vector<Vertex> vertices;
	vertices.resize(res.attributes.positions.size());

	std::vector<uint16_t> indices;
	std::vector<uint16_t> submeshesIndices;
	std::vector<uint16_t> submeshesMat;
	uint16_t lastIndex = 0;


	for (const auto& shape : res.shapes) {
		
		submeshesIndices.push_back(indices.size());
		submeshesMat.push_back(shape.mesh.material_ids[0]); // THIS IS TRASH BUT I DONT WANT TO DO PER FACE

		for (rapidobj::Index i : shape.mesh.indices)
		{
			indices.push_back(i.position_index);

			key currentKey =
			{
				i.position_index,
				i.normal_index,
				i.texcoord_index
			};

			if (cachedKeys.contains(currentKey)) continue;
			
			cachedKeys.insert(std::move(currentKey));
			vertices[i.position_index] = std::move(generateVertexFromKey(i, res));
		}



	}

	submeshesIndices.push_back(static_cast<uint16_t>(indices.size()));

	// -- Build Materials

	std::vector<Material> mats;

	for (const auto& mat : res.materials)
	{
		Material resMat;
		MaterialCoefs mc;

		mc.Ka[0] = mat.ambient[0];
		mc.Ka[1] = mat.ambient[1];
		mc.Ka[2] = mat.ambient[2];

		mc.Kd[0] = mat.diffuse[0];
		mc.Kd[1] = mat.diffuse[1];
		mc.Kd[2] = mat.diffuse[2];

		mc.Ks[0] = mat.specular[0];
		mc.Ks[1] = mat.specular[1];
		mc.Ks[2] = mat.specular[2];


		mc.Ke[0] = mat.emission[0];
		mc.Ke[1] = mat.emission[1];
		mc.Ke[2] = mat.emission[2];


		mc.Ns = mat.shininess;
		mc.transparency = 1.f - mat.dissolve;
		mc.Ni = mat.ior;

		resMat.setCoefficients(mc);
		resMat.setModel(mat.illum);
		mats.emplace_back(resMat);

	}
	
	//Extremely weird bug

	return Mesh(vertices, indices, submeshesIndices, mats, submeshesMat);//, mats, submeshesMat);
}
