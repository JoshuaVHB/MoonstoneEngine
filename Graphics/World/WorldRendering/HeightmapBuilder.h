#pragma once

#include "../Mesh.h"
#include "Heightmap.h"
#include <directXmath.h>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <filesystem>

namespace fs = std::filesystem;

using Index = uint32_t;
using Vec = XMVECTOR;

/// <summary>
/// This is taken from the first rush with a bit of rewrite (changing types names, i hate dxmath)
/// </summary>

class HeightmapBuilder {

public:

	static Mesh buildTerrainMesh(const Heightmap& map, float xy_scale = 1, float height_factor = 1) {

		int width = map.getWidth();
		int height = map.getHeigth();

		Mesh terrainMesh;

		std::vector<Vertex> vertices;
		std::vector<Index> indices;

		// -- Step 1 : Build the vertices

		for (int y = 0; y < height; y++)
			for (int x = 0; x < width; x++)
			{
				// 1.1 -> Compute the position
				Vertex v;
				float h = map.getAt(x, y);
				v.position = Vec{ x * xy_scale,h * height_factor*xy_scale  ,y * xy_scale, };

				// 1.2 -> Compute the normal
				float a, b, c, d;
				a = ((y + 1) < height) ? map.getAt(x, y + 1) :	XMVectorGetY(v.position);
				b = ((y - 1) >= 0) ? map.getAt(x, y - 1)	:	XMVectorGetY(v.position);
				c = ((x + 1) < width) ? map.getAt(x + 1, y) :	XMVectorGetY(v.position);
				d = ((x - 1) >= 0) ? map.getAt(x - 1, y)	:	XMVectorGetY(v.position);

				Vec normal = XMVector4Normalize(XMVector3Cross(
					Vec{ 0.f, (a - b) * height_factor, 2.f * xy_scale },
					Vec{ 2.f * xy_scale, (c - d) * xy_scale, 0.f }
				));

				v.normal = Vec{ normal };

				// 1.3 -> Compute UVs

				Vec uv = { (float)x, (float)y };
				v.uv = uv;

				vertices.push_back(v);
			}

		// -- Step 2 : Build the indicies
		for (int y = 0; y < height - 1; y++)
			for (int x = 0; x < width - 1; x++)
			{

				Index a1 = y		* (width) + x;
				Index a2 = y		* (width) + x + 1;
				Index a3 = (y + 1)	* (width) + x;
				Index a4 = (y + 1)	* (width) + x + 1;
				indices.push_back(a1);
				indices.push_back(a3);
				indices.push_back(a2);
				indices.push_back(a2);
				indices.push_back(a3);
				indices.push_back(a4);

			}


		return Mesh(vertices, indices);


	}
};

class HeightmapReader {
public:
	/* Reads an image an produces a Heightmap */
	static Heightmap readMapFromFile(const fs::path& path)
	{
		int x, y, channel;

		unsigned char* data = stbi_load(path.string().c_str(), &x, &y, &channel, 1);
		if (!data) throw std::exception();

		Heightmap resMap(x, y);

		// Store the values of the heightmap png into the object while normalizing them
		std::transform(resMap.begin(), resMap.end(), resMap.begin(),
			[ptr = data](float& v) mutable {return static_cast<float>((*ptr++) / 255.f); });

		stbi_image_free(data);

		return resMap;

	}
};