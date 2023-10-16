#pragma once


#include <vector>
#include <memory>
#include <variant>

#include "Lights.h"
#include "../../abstraction/Shaders.h"

/* Add this to the shader code to attach lights correctly :

struct DirectionalLight {
	float4 direction;
	float4 ambiant;
	float4 diffuse;
	float	strength;
};

struct PointLight {

	float4 range;
	float4 position;
	float4 ambiant;
	float4 diffuse;
	float falloff;
};


struct SpotLight {

	float4 direction;
	float4 ambiant;
	float4 diffuse;
	float4 position;

	float falloff;
	float radius;
	float strength;
};

cbuffer LightManager {

	Spotlight			spotlights[64];
	PointLight			pointslights[64];
	DirectionalLight	dirlights[64];
};*/

class LightManager 
{
private:

	std::vector<DirectionalLight> m_dirs;
	std::vector<PointLight> m_point;
	std::vector<SpotLight> m_spots;

	std::unique_ptr<Effect> m_effect;

public:

	LightManager(const Effect& e)
		: m_effect(std::make_unique<Effect>(e))
	{
	
	
	}

	void addSpotlight()		{}
	void addDirlight()		{}
	void addPointlight()	{}

	void setupBuffersForEffect(Effect& effect)
	{
	}


	void sendLightsToEffect(Effect& effect)
	{
	}

public:

	void showDebugWindow() {

	}

};