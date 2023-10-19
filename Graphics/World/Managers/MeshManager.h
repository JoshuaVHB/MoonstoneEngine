#pragma once

#include <filesystem>
namespace fs = std::filesystem;


class Mesh;
class Camera;

class MeshManager
{
	
public:
	static Mesh loadMeshFromFile(const fs::path& pathToFile);

public:
	static void clear();
	static void feed(const Mesh& mesh);
	static void render(const Camera& camera);
	static void onImGuiRender();
};

