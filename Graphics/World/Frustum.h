#pragma once

#define _XM_NO_INTRINSICS_
#include <directXmath.h>
#include "../abstraction/Camera.h"
#include "../../Utils/AABB.h"

struct Plan {

    DirectX::XMVECTOR normal;
    float distanceToOrigin;

    Plan() :
        normal(), distanceToOrigin(0)
    {}

    Plan(const DirectX::XMVECTOR& pl, const DirectX::XMVECTOR& norm) :
        normal(XMVector3Normalize(norm)),
        distanceToOrigin(XMVectorGetX(XMVector3Dot(normal, pl)))
    {}

    inline float getSDToPlan(const DirectX::XMVECTOR& point) const {
        return XMVectorGetX(XMVector3Dot(normal, point)) - distanceToOrigin;
    }
};

struct Frustum {

    Plan topFace;
    Plan bottomFace;

    Plan rightFace;
    Plan leftFace;

    Plan farFace;
    Plan nearFace;

    bool            isOnFrustum(const AABB& boundingBox) const;
    static Frustum  createFrustumFromPerspectiveCamera(const Camera& cam);
    static bool     isOnOrForwardPlan(const Plan& plan, const AABB& boundingBox);
};
