#pragma once

#include "../Mesh.h"
#include "Heightmap.h"
#include "HeightmapBuilder.h"
#include <map>
#include <filesystem>

namespace fs = std::filesystem;

struct Chunk
{
	Mesh chunkMesh;
};

class Terrain {


public:

	struct TerrainParams {

		int		width, height;
		float	xyScale;
		float	scaleFactor;
		int		chunkSize;
		Vec2<int> chunkCount{1,1};
	};

private:


	TerrainParams m_params;

	Heightmap m_map;
	//Mesh m_mesh;
	std::vector<Mesh> m_chunks;

	fs::path m_path; // used for hot reload

public:


	Terrain(const fs::path& pathToMap) 
	{
		m_path = pathToMap;
		m_map = HeightmapReader::readMapFromFile(pathToMap);

		const TerrainParams defaultParams{
			m_map.getWidth(), m_map.getHeigth(),
			/*xyScale*/ 1.F,
			/*height scale*/ 100.F,
			1
		};

		m_params = defaultParams;

		rebuildMesh();
	}

	void changeMap(const fs::path& pathToMap) {
		m_map = HeightmapReader::readMapFromFile(pathToMap);
		rebuildMesh();
	}

	void hotReloadMap()
	{
		changeMap(m_path);
	}

	void rebuildMesh() {
		m_chunks = HeightmapBuilder::buildTerrainMesh(
			m_map, m_params.chunkCount, 
			m_params.xyScale, m_params.scaleFactor);
	}

	void setParams(const TerrainParams &params) {
		m_params = params;
		rebuildMesh();
	}

	const TerrainParams& getParams() const { return m_params; }

	//const Mesh& getMesh() const noexcept { return m_mesh; }
	//Mesh& getMesh() noexcept { return m_mesh; }
	std::vector<Mesh>& getMesh() noexcept { return m_chunks; }


	void showDebugWindow() {

		ImGui::Begin("Terrain settings");

		if (
			ImGui::DragFloat("XY Scale", &m_params.xyScale) +
			ImGui::DragFloat("Height Factor", &m_params.scaleFactor) +
			ImGui::DragInt2("Height Factor", &m_params.chunkCount[0], 1, 1, 10)
			) {
			rebuildMesh();
		}


		ImGui::End();


	}



};