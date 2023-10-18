#pragma once

#include <DirectXMath.h>

using namespace DirectX;
using Vec = XMVECTOR;


struct BoundingVolume {};


struct AABB : BoundingVolume
{
	Vec origin;
	Vec size;


	bool isInBounds(Vec point) {
		return
			XMVectorGetX(point) > XMVectorGetX(origin) && XMVectorGetX(point) < XMVectorGetX(origin) + XMVectorGetX(size) &&
			XMVectorGetY(point) > XMVectorGetY(origin) && XMVectorGetY(point) <  XMVectorGetY(origin) + XMVectorGetY(size) &&
			XMVectorGetZ(point) > XMVectorGetZ(origin) && XMVectorGetZ(point) < XMVectorGetZ(origin) + XMVectorGetZ(size);
	}

	static bool overlaps(AABB first, AABB second)
	{
		return
		XMVectorGetX(first.origin) < XMVectorGetX(second.origin) + XMVectorGetX(second.size) && XMVectorGetX(first.origin) + XMVectorGetX(second.origin) > XMVectorGetX(second.origin) &&
		XMVectorGetY(first.origin) < XMVectorGetY(second.origin) + XMVectorGetY(second.size) && XMVectorGetY(first.origin) + XMVectorGetY(second.origin) > XMVectorGetY(second.origin) &&
		XMVectorGetZ(first.origin) < XMVectorGetZ(second.origin) + XMVectorGetZ(second.size) && XMVectorGetZ(first.origin) + XMVectorGetZ(second.origin) > XMVectorGetZ(second.origin);

	}

};