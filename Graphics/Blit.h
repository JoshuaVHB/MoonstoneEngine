#pragma once

#include "abstraction/Texture.h"
#include "abstraction/Effect.h"

class Blit
{
private:

	Effect m_blitFx;

public:


	Blit()
	{
		m_blitFx.loadEffectFromFile("res/effects/blit.fx");
	}


	void blitTextureToScreen(const Texture& tex)
	{

		m_blitFx.bindTexture("source", tex);
		m_blitFx.apply();
	}
};
