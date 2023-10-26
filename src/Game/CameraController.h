#pragma once

#include "Cloporte.h"
#include "abstraction/Camera.h"

class CameraController
{
public:
	static void computeThirdPersonPosition(const Cloporte& player, Camera& cam)
	{

        static float distance = 3;
        auto forward = player.getForward();
        auto pos = player.getPosition();

        DirectX::XMVECTOR camPos = XMVectorMultiply(XMVectorSubtract(pos, forward), XMVECTOR{ 3,3,3 });
        camPos = XMVectorAdd(camPos, { 0,distance,0 });
        cam.setPosition(camPos);
        cam.lookAt(pos);
        cam.updateCam();

	}


};

