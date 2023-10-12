#pragma once

#include <filesystem>
namespace fs = std::filesystem;


class Mesh;

class MeshManager
{
public:
	static Mesh loadMeshFromFile(const fs::path& pathToFile);

};

