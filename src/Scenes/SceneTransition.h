﻿#pragma once

#include "Renderer.h"
#include "abstraction/Texture.h"
#include "Scene.h"
#include "abstraction/Effect.h"
#include "abstraction/FrameBuffer.h"

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
	Scene* from;
	Scene* to;
	TransitionCurve curve;
	TransitionType type;

	Effect transitionEffect;
	FrameBuffer originTextureFBO;
	FrameBuffer endTextureFBO{1};

	float currentTime;
	float timeDuration;
	bool firstDraw = true;

public:


	// get two textures of the scene

	SceneTransition(Scene* from,
		Scene* to,
		const TransitionCurve curve,
		const TransitionType type)
		: from(from)
		, to(to)
		, curve(curve)
		, type(type)
		, timeDuration(1)
		, currentTime(0)
	{

		transitionEffect.loadEffectFromFile("res/effects/transition.fx");

	}
	
	void setDuration(float dur)
	{
		timeDuration = dur;
	}


	bool step(float deltaTime)
	{
		Renderer::setBackbufferToDefault();
		if (firstDraw)
		{
			deltaTime = 0;
			firstDraw = false;
		}

		if (!from || !to) return true;
		currentTime += deltaTime;



		originTextureFBO.bind();
		from->onUpdate(deltaTime);
		from->onRender();
		originTextureFBO.unbind();

		Renderer::setBackbufferToDefault();

		endTextureFBO.bind();
		to->onUpdate(deltaTime);
		to->onRender();
		endTextureFBO.unbind();

		Renderer::setBackbufferToDefault();

		transitionEffect.bindTexture("origin", originTextureFBO.getResource());
		transitionEffect.bindTexture("end", endTextureFBO.getResource());

		transitionEffect.setUniformVector("u_time", { currentTime, timeDuration });
		transitionEffect.apply();

		Renderer::setBackbufferToDefault();
		Renderer::draw(6);

		originTextureFBO.clear();
		endTextureFBO.clear();
		// return true when transition is done
		return currentTime < timeDuration;
	}

	void release()
	{
		delete from;
	}

};
