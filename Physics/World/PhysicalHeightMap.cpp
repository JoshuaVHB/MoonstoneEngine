#include "PhysicalHeightMap.h"
#include <algorithm>

#include "../../Graphics/World/WorldRendering/Terrain.h"
#include "../../Graphics/World/WorldRendering/Heightmap.h"
void PhysicalHeightMap::setTerrain(Terrain* _terrain)
{
	terrain.reset(_terrain);
	Heightmap hmap = _terrain->getHeightmap();
	Terrain::TerrainParams p = terrain->getParams();
	PhysicsEngine::HeightMapData data;


	data.length = hmap.getHeigth();
	data.width = hmap.getWidth();
	data.scaleXZ = p.xyScale;
	data.scaleY = p.scaleFactor;
	data.maxHeight = hmap.getMaxHeight();
	std::for_each(
		hmap.begin(),
		hmap.end(),
		[&data](float& height) {
			data.value.emplace_back(height);
		}
	);

	bool success = PhysicsEngine::addHeightmap(id, data);



}
