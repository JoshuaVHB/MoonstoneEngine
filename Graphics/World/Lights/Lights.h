#pragma once

#include <directXmath.h>

using namespace DirectX;

using Vec = XMVECTOR;

enum class LightType : int32_t
{
	DIRECTIONAL,
	POINT,
	SPOTLIGHT,
};

struct Light
{
	bool isOn=false;
	LightType type;
};

struct hlsl_GenericLight
{

	Vec		direction; // For directional lights and spotlight
	Vec		range;
	Vec		position;
	Vec		ambiant;
	Vec		diffuse;

	float falloff;
	float radius;
	float strength;
	float isOn;

	uint32_t type;

	float padding[3];

};

struct DirectionalLight : public Light {

	Vec direction;
	Vec ambiant;
	Vec diffuse;
	LightType type = LightType::DIRECTIONAL;

	float strength;
private:

	float padding[2];
};


struct PointLight : public Light {

	Vec range;
	Vec position;
	Vec ambiant;
	Vec diffuse;
	float falloff;
	LightType type = LightType::POINT;

	PointLight(Vec r, Vec pos, Vec Ka, Vec Kd, float f, bool on)
	{
		range = r;
		position=pos;
		ambiant = Ka;
		diffuse = Kd;
		falloff = f;
		isOn = on;
	}


	float padding[2];

};

struct SpotLight : public  Light{

	Vec direction;
	Vec ambiant;
	Vec diffuse;
	Vec position;
	LightType type = LightType::SPOTLIGHT;

	float falloff;
	float radius;
	float strength;
};