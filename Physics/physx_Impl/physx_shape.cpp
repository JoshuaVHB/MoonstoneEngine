#include "physx_shape.h"
#include "physx_impl.h"
#include <cassert>
#include <PxPhysicsAPI.h>

PxShape* physx_shape::getCube(const PhysicsEngine::fVec3 scale)
{
	Physx_Impl::ModulePhysics& mp = Physx_Impl::getModulePhysics();
	PxPhysics* physics = mp.gPhysics;
	PxMaterial* material = mp.gMaterial;
    PxShape* shape = physics->createShape(PxBoxGeometry(scale.x, scale.y, scale.z), *material);
    return shape;
}

PxShape* physx_shape::getBall(const float scale)
{
	Physx_Impl::ModulePhysics& mp = Physx_Impl::getModulePhysics();
	PxPhysics* physics = mp.gPhysics;
	PxMaterial* material = mp.gMaterial;
    PxShape* shape = physics->createShape(PxSphereGeometry(scale), *material);
    return shape;
}

PxShape* physx_shape::getHeightmap(const PhysicsEngine::HeightMapData& data)
{
	Physx_Impl::ModulePhysics& mp = Physx_Impl::getModulePhysics();
	PxPhysics* physics = mp.gPhysics;
	PxMaterial* material = mp.gMaterial;
	assert(data.value.size() == data.width * data.length);
	
	PxHeightFieldSample* hfSamples = new PxHeightFieldSample[data.length * data.width];
	
	PxU32 index = 0;
	for (PxU32 col = 0; col < data.length; col++)
	{
		for (PxU32 row = 0; row < data.width; row++)
		{
	
			PxHeightFieldSample& smp = hfSamples[(row * data.width) + col];
			smp.height = PxI16(data.value[(col * data.width) + row] * 255);
		}
	}
	
	// Build PxHeightFieldDesc from samples
	PxHeightFieldDesc terrainDesc;
	terrainDesc.format = PxHeightFieldFormat::eS16_TM;
	terrainDesc.nbColumns = data.length;
	terrainDesc.nbRows = data.width;
	terrainDesc.samples.data = hfSamples;
	terrainDesc.samples.stride = sizeof(PxHeightFieldSample);
	terrainDesc.flags = PxHeightFieldFlags();
	PxHeightField* aHeightField = PxCreateHeightField(terrainDesc,	physics->getPhysicsInsertionCallback());
	
	
	PxHeightFieldGeometry hfGeom;
	hfGeom.columnScale = data.scaleXZ;
	hfGeom.rowScale = data.scaleXZ;
	hfGeom.heightScale = data.scaleY / 255.f;
	hfGeom.heightField = aHeightField;
	hfGeom.heightFieldFlags = PxMeshGeometryFlag::eDOUBLE_SIDED;
	
	delete[] hfSamples;
	
	PxShape* shape = physics->createShape(hfGeom, *material);
	return shape;
}
