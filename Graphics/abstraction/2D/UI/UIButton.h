#pragma once

#include "UIFrame.h"

// should be a x,y,size with behaviour on click
// should also highlight on hover
// should contain text


#include "UIElement.h"
#include "abstraction/2D/Renderer2D.h"

struct UIButton : public UIElement
{


	int x, y;
	int width, height;
	UIFrame* m_parent = nullptr;
	bool hovered = false;
	

public:

	bool isHovered(int mouseX, int mouseY) const
	{
		return (mouseX > x && mouseX < x + width) &&
			(mouseY > y && mouseY < y + height);
	}

	bool isClicked(int mouseX, int mouseY, bool pressed) const 
	{
		return (pressed) && isHovered(mouseX, mouseY);
	}


	virtual void render(Renderer2D& renderer) override
	{

		static 	Texture baseTexture{ "res/textures/UIBox.dds" };
		static Texture hoveredTexture{ "res/textures/UIBox_hovered.dds" };
		const Texture& currentTexture = (hovered) ? hoveredTexture : baseTexture;
		renderer.batchQuad({ (float)x,(float)y }, { (float)width, (float)height }, currentTexture);
	}

};
