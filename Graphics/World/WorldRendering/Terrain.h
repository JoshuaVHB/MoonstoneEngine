#pragma once

#include "../Mesh.h"
#include "Heightmap.h"
#include "HeightmapBuilder.h"
#include <filesystem>

namespace fs = std::filesystem;

class Terrain {


public:

	struct TerrainParams {

		int		width, height;
		float	xyScale;
		float	scaleFactor;
		int		chunkSize;
	};

private:


	TerrainParams m_params;

	Heightmap m_map;
	Mesh m_mesh;


	fs::path m_path; // used for hot reload

public:


	Terrain(const fs::path& pathToMap) 
	{
		m_path = pathToMap;
		m_map = std::move(HeightmapReader::readMapFromFile(pathToMap));

		TerrainParams defaultParams{
			m_map.getWidth(), m_map.getHeigth(),
			/*xyScale*/ 1.F,
			/*height scale*/ 100.F,
			1.F
		};

		m_params = std::move(defaultParams);

		rebuildMesh();
	}

	void changeMap(const fs::path& pathToMap) {
		m_map = std::move(HeightmapReader::readMapFromFile(pathToMap));
		rebuildMesh();
	}

	void hotreloadMap()
	{
		changeMap(m_path);
	}

	void rebuildMesh() {
		m_mesh = HeightmapBuilder::buildTerrainMesh(m_map, m_params.xyScale, m_params.scaleFactor);
	}

	void setParams(TerrainParams params) {
		m_params = params;
		rebuildMesh();
	}

	TerrainParams getParams() const { return m_params; }

	const Mesh& getMesh() const noexcept { return m_mesh; }
	Mesh& getMesh() noexcept { return m_mesh; }


	void showDebugWindow() {

		ImGui::Begin("Terrain settings");

		if (
			ImGui::DragFloat("XY Scale", &m_params.xyScale) +
			ImGui::DragFloat("Height Factor", &m_params.scaleFactor) 
			) {
			rebuildMesh();
		}


		ImGui::End();


	}



private:

	struct Chunk {
		// This will come later
	};

	void generateChunks() {}



};