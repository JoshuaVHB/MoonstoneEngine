#pragma once

#include "../abstraction/Camera.h"
#include "../../Utils/AABB.h"

#include <directXmath.h>
using namespace DirectX;
using Vec = XMVECTOR;

struct Plan {

    Vec normal;
    float distanceToOrigin;

    Plan() :
        normal(), distanceToOrigin(0)
    {}

    Plan(const Vec& pl, const Vec norm) :
        normal(XMVector3Normalize(norm)),
        distanceToOrigin(XMVectorGetX(XMVector3Dot(normal, pl)))
    {}

    inline float getSDToPlan(const Vec& point) const {
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

    bool isOnFrustum(const AABB& boudingBox) const;

    static Frustum createFrustumFromCamera(const Camera& cam);
    static Frustum createFrustumFromOrthographicCamera(const Camera& cam);
    static Frustum createFrustumFromPerspectiveCamera(const Camera& cam);

    static bool isOnOrForwardPlan(const Plan& plan, const AABB& boundingBox);
};

Frustum Frustum::createFrustumFromPerspectiveCamera(const Camera& cam)
{
    
    //PerspectiveProjection proj{};
    Frustum frustum;

    float zfar = -1000.F;
    float znear  = -0.10F;
    float fovy  = 3.1415f/4.F;
    float aspect = 2.0f;

    const float halfVSide = zfar *tanf(fovy * .5f);
    const float halfHSide = halfVSide * aspect;
    const Vec frontMultFar = zfar * cam.getForward();

    Vec hz = cam.getHorizontalDir();
    Vec fw = cam.getForward();
    Vec up = cam.getUp();
    Vec campos = cam.getPosition();

    frustum.nearFace = { campos + znear * fw, -fw };
    frustum.farFace = { campos + frontMultFar, fw };

    frustum.rightFace = {   campos, XMVector3Cross(up, frontMultFar + hz * halfHSide) };
    frustum.leftFace = {    campos, XMVector3Cross(frontMultFar - hz * halfHSide, up) };
    frustum.topFace = {     campos, XMVector3Cross(hz, frontMultFar - up * halfVSide) };
    frustum.bottomFace = {  campos, XMVector3Cross(frontMultFar + up * halfVSide, hz) };

    return frustum;
    
}

bool Frustum::isOnFrustum(const AABB& boudingBox) const
{
    bool l=false, r = false, t = false, b = false, n = false, f = false;
    std::string print = "";

    if (isOnOrForwardPlan(leftFace, boudingBox))        print += "l";
    if (isOnOrForwardPlan(rightFace, boudingBox))       print += "r";
    if (isOnOrForwardPlan(topFace, boudingBox))         print += "t";
    if (isOnOrForwardPlan(bottomFace, boudingBox))      print += "b";
    if (isOnOrForwardPlan(nearFace, boudingBox))        print += "n";
    if (isOnOrForwardPlan(farFace, boudingBox))         print += "f";

    std::cout << print << std::endl;

    return (
        isOnOrForwardPlan(leftFace, boudingBox) &&
        isOnOrForwardPlan(rightFace, boudingBox) &&
        isOnOrForwardPlan(topFace, boudingBox) &&
        isOnOrForwardPlan(bottomFace, boudingBox) &&
        isOnOrForwardPlan(nearFace, boudingBox) &&
        isOnOrForwardPlan(farFace, boudingBox)
        );
}

bool Frustum::isOnOrForwardPlan(const Plan& plan, const AABB& boundingBox)
{
    // Compute the projection interval radius of b onto L(t) = b.c + t * p.n

    Vec center = XMVectorAdd(boundingBox.origin, XMVectorDivide(boundingBox.size, Vec{ 2,2,2,2 })) ;
    Vec e =         XMVectorSubtract(XMVectorAdd(boundingBox.origin, boundingBox.size), center);
    const float r = 
        XMVectorGetX(e) * std::abs(XMVectorGetX(plan.normal)) 
        + XMVectorGetY(e) * std::abs(XMVectorGetY(plan.normal)) 
        + XMVectorGetZ(e) * std::abs(XMVectorGetZ(plan.normal));
    return -r <= plan.getSDToPlan(center);
}