#pragma once

#include <DirectXMath.h>

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