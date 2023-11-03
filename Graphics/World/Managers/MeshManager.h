#pragma once

#include <filesystem>
#include <vector>
#include "../Mesh.h"
#include "../Frustum.h"
#include "../../abstraction/Camera.h"
namespace fs = std::filesystem;



class MeshManager
{
private:

	static std::vector<const Mesh*> m_meshes;


public:
	static Mesh loadMeshFromFile(const fs::path& pathToFile);



public:
	static void clear()
	{
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

