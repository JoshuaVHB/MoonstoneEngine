#pragma once

#include "UIFrame.h"

// should be a x,y,size with behaviour on click
// should also highlight on hover
// should contain text


#include "UIElement.h"

struct UIButton : public UIElement
{



	int x, y;
	int width, height;
	UIFrame* m_parent = nullptr;

	

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


	void render()
	{
		
	}

};
