#pragma once

#include <filesystem>
#include <vector>
#include <unordered_map>
#include <memory>
#include <string>
#include "../Mesh.h"
#include "../Frustum.h"
#include "../../abstraction/Camera.h"
namespace fs = std::filesystem;



class MeshManager
{
private:

	static std::vector<const Mesh*> m_meshes;
	static std::unordered_map<std::string, std::unique_ptr<Mesh>> s_loadedMeshes;

public:

	static Mesh loadMeshFromFile(const fs::path& pathToFile);

	static void loadMeshFromFile(const std::string& name, const fs::path& pathToFile);



public:

	static void unloadMesh(const std::string& name)
	{
		s_loadedMeshes.erase(name);
	}

	static void clear()
	{
		std::ranges::for_each(m_meshes, [&](auto* m) {delete m; });
		m_meshes.clear();
	}
	static void feed(const Mesh* mesh)
	{
		m_meshes.push_back(mesh);
	}

	//template<class renderFn>
	static void render(Camera& camera, const std::function<void(Camera& cam, const Mesh& m)>& renderFn)

	{
		Frustum frustum = Frustum::createFrustumFromPerspectiveCamera(camera);

		std::for_each(
			m_meshes.begin(),
			m_meshes.end(),
			[&](const Mesh* m) {
				if (frustum.isOnFrustum(m->getBoundingBox()))
				{
					renderFn(camera, *m);			
				}
			}
		);
	}
	static void onImGuiRender();
};

