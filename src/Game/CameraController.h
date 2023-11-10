#pragma once

#include "Cloporte.h"
#include "abstraction/Camera.h"
#include "World/WorldRendering/Terrain.h"

inline float smoothstep(float a, float b, float x)
{
    float t = std::clamp((x - a) / (b - a), 0.F, 1.f );
    return t * t * (3.0f - (2.0f * t));
}

class CameraController
{

private:

    static const Terrain* terrain; // HORRIBLE

public:


    void setOldPlayerDirection();

	static void computeThirdPersonPosition(const Cloporte& player, Camera& cam)
	{

		static const float distanceFromPlayer = 25.F;
        auto forward = player.getForward();
        auto pos = player.getPosition();
        auto gdir = player.getGroundDir();


        cam.setRoll(computeCameraRoll(player, cam));//+ DirectX::XM_PIDIV2);
        float h = std::clamp(computeCameraHeight(player, cam), -30.f, 30.f);
        
        
        DirectX::XMVECTOR camPos = pos - (distanceFromPlayer * forward) + XMVECTOR{0, -h, 0}; //??

        static const float MAX_DISTANCE = 20.f;
        camPos = XMVectorLerp(
            cam.getPosition(), 
            camPos, 
            smoothstep(0, MAX_DISTANCE, XMVectorGetX(XMVector3Length(camPos - cam.getPosition())))
            );

        cam.setPosition(camPos);
        cam.lookAt(pos);
        cam.updateCam();

	}



    static float computeCameraRoll(const Cloporte& player, Camera& cam)
	{
        auto forward = player.getForward();
        auto gdir = player.getGroundDir();

        auto vec_chelou_1 = XMVector3Cross(forward, { 0,1,0 });
        auto vec_chelou_2 = XMVector3Cross(forward, vec_chelou_1);

        // le plan (vec_chelou_1 , vec_chelou_2) a pour normale forward;
        // on projet gdir sur le plan_chelou

        auto proj_x_chelou = XMVectorGetX(XMVector3Dot(gdir, vec_chelou_1));
        auto proj_y_chelou = XMVectorGetX(XMVector3Dot(gdir, vec_chelou_2));

        // On applique l'arctan pour avoir l'angle
        static const float MAX_ROLL_ANGLE = 10.F;
        float old_roll = cam.getRoll();
        float roll = std::atan2f(proj_y_chelou, proj_x_chelou);

        float computedRoll = std::lerp(
            old_roll,
            roll + DirectX::XM_PIDIV2,
            .05f
        );

        return computedRoll;
	}

    static void setTerrain(const Terrain* t)
	{
        terrain = t;
	}

    static float computeCameraHeight(const Cloporte& player, Camera& cam)
    {
        auto forward = player.getForward();
        auto pos = player.getPosition();
        float height = XMVectorGetY(pos);
        static const float STEP_LENGTH = 5;

        // sample height in front and check height difference
        auto posInFront = pos + forward * STEP_LENGTH;
        float trueHeight = terrain->getWorldHeightAt(posInFront);
        float delta = trueHeight - XMVectorGetY(posInFront); // > 0 si le point est sous la map, 

        float camHeight = XMVectorGetY(cam.getPosition());

        return delta;
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

