#pragma once
#include "Scene.h"

enum TransitionType
{
	FADE_TO_BLACK,
	LEFT_RIGHT_SWEEP,
	FADE,
};

enum class TransitionCurve
{
	LINEAR,
	SMOOTHSTEP,
};

class SceneTransition
{
private:

	// have the effect
	const Scene* from;
	const Scene* to;

public:


	// get two textures of the scene

	void createTransition(const Scene* from, const Scene* to, TransitionType transition, TransitionCurve curve)
	{
		
	}

	bool transit(float deltaTime)
	{
		// return true when transition is done
		return false;
	}


};
