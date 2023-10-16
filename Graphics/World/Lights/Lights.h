#pragma once

#include <directXmath.h>

using namespace DirectX;

using Vec = XMVECTOR;

struct Light {};

struct DirectionalLight : public Light {

	Vec direction;
	Vec ambiant;
	Vec diffuse;

	float strength;
private:

	float padding[3];
};


struct PointLight : public Light {

	Vec range;
	Vec position;
	Vec ambiant;
	Vec diffuse;
	float falloff;

private:

	float padding[3];
};

struct SpotLight {

	Vec direction;
	Vec ambiant;
	Vec diffuse;
	Vec position;

	float falloff;
	float radius;
	float strength;
private:

	float padding[1];
};