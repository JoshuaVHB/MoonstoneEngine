#pragma once

#include "Cloporte.h"
#include "abstraction/Camera.h"

inline float smoothstep(float a, float b, float x)
{
    float t = std::clamp((x - a) / (b - a), 0.F, 1.f );
    return t * t * (3.0 - (2.0 * t));
}

class CameraController
{

private:



public:

    void setOldPlayerDirection();

	static void computeThirdPersonPosition(const Cloporte& player, Camera& cam)
	{

		static const float distanceFromPlayer = 25.F;
        auto forward = player.getForward();
        auto pos = player.getPosition();
        
        DirectX::XMVECTOR camPos = pos - (distanceFromPlayer * forward) + XMVECTOR{0, 8, 0};
        //camPos = XMVectorAdd(camPos, { 0,distance,0 });
        static const float MAX_DISTANCE = 200.f;
        camPos = XMVectorLerp(
            cam.getPosition(), 
            camPos, 
            smoothstep(0, MAX_DISTANCE, XMVectorGetX(XMVector3Length(camPos - cam.getPosition()))
            ));

        cam.setPosition(camPos);
        cam.lookAt(pos);
        cam.updateCam();

	}

    static void computeFirstPersonPosition(const Cloporte& player, Camera& cam)
    {

        cam.setPosition(player.getPosition() + XMVECTOR{0,2,0});
        static const float MAX_SLERP = 10.f;
        XMVECTOR lerpedForward = XMVectorLerp(
            cam.getForwardDir(),
            player.getForward(),
            smoothstep(0, 1, XMVectorGetX(XMVector3Length(player.getForward() - cam.getForwardDir() ))
            ));

        cam.lookAt(lerpedForward + player.getPosition() + XMVECTOR{ 0,2,0 });
        cam.updateCam();

    }


};

