#pragma once

#define _XM_NO_INTRINSICS_
#include <algorithm>
#include <DirectXMath.h>
#include <array>

using namespace DirectX;
using Vec = XMVECTOR;


struct BoundingVolume {};
struct AABB;

struct BoundingSphere : BoundingVolume
{
	Vec origin;
	float radius;

	BoundingSphere() = default;
	BoundingSphere(const Vec& center, float r) : origin(center), radius(r) {}

};


struct AABB : BoundingVolume
{
	Vec origin{};
	Vec size{};

	AABB() = default;
	AABB(const Vec& _origin, const Vec& _size) 
		: origin(_origin)
		, size(_size)
	{}
	AABB(const AABB& other)
		: origin(other.origin)
		, size(other.size)
	{}

	AABB& operator=(const AABB& other)
	{
		origin = other.origin;
		size = other.size;
		return *this;
	}

	static AABB makeAABBFromPoints(Vec a, Vec b)
	{
		Vec origin = XMVectorMin(a, b);
		Vec size = XMVectorAbs(XMVectorSubtract(b,a));
		return AABB(origin, size);
	}

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

	std::array<XMVECTOR, 8> getPoints() const
	{
		return {
			origin,
			origin + XMVECTOR{XMVectorGetX(size),	0,					0					},
			origin + XMVECTOR{0,					0,					XMVectorGetZ(size)	},
			origin + XMVECTOR{XMVectorGetX(size),	0,					XMVectorGetZ(size)	},
			origin + XMVECTOR{0,					XMVectorGetY(size) ,0					},
			origin + XMVECTOR{XMVectorGetX(size),	XMVectorGetY(size), 0					},
			origin + XMVECTOR{0,					XMVectorGetY(size), XMVectorGetZ(size)	},
			origin + size

		};

	}

};


static BoundingSphere makeSphereFromAABB(AABB aabb)
{
	Vec center = XMVectorAdd(aabb.origin, XMVectorDivide(aabb.size, XMVECTOR{ 2,2,2,2 }));
	float diagLen = XMVectorGetX(XMVector3Length(XMVectorSubtract(center, XMVectorAdd(aabb.origin, aabb.size))));


	return BoundingSphere{ center, diagLen };
}

static AABB makeAABBFromSphere(const BoundingSphere& sphere)
{
	Vec A = XMVectorSubtract(sphere.origin, Vec{ sphere.radius,sphere.radius,sphere.radius });
	Vec B = XMVectorAdd(Vec{ sphere.radius,sphere.radius,sphere.radius }, sphere.origin);
	return AABB::makeAABBFromPoints(A, B);
}


static float SquareDistanceToAABB(XMVECTOR point, const AABB& aabb)
{
	float sqDist = 0.0f;

	auto vertices = aabb.getPoints();

	auto minAxes = [&](int index) 
	{
		return XMVectorGetByIndex(*std::ranges::min_element(vertices, [&](XMVECTOR a, XMVECTOR b)->bool
			{
				return XMVectorGetByIndex(a, index) < XMVectorGetByIndex(b, index);
			}), index);
	};

	auto maxAxes = [&](int index)
	{
		return XMVectorGetByIndex(*std::ranges::max_element(vertices, [&](XMVECTOR a, XMVECTOR b)->bool
			{
				return XMVectorGetByIndex(a, index) < XMVectorGetByIndex(b, index);
			}), index);
	};


	for (int i = 0; i < 3; i++) {
		// for each axis count any excess distance outside box extents
		float v = XMVectorGetByIndex(point, i);


		if (v < minAxes(i)) sqDist += (minAxes(i) - v) * (minAxes(i) - v);
		if (v > maxAxes(i)) sqDist += (v - maxAxes(i)) * (v - maxAxes(i));
	}
	return sqDist;
}


static bool SphereAABBTest(const BoundingSphere& sphere, const AABB& aabb)
{
	// Compute squared distance between sphere center and AABB
   // the sqrt(dist) is fine to use as well, but this is faster.
	float sqDist = SquareDistanceToAABB(sphere.origin, aabb);

	// Sphere and AABB intersect if the (squared) distance between them is
	// less than the (squared) sphere radius.
	return sqDist <= sphere.radius * sphere.radius;
}