#pragma once

#include <stdexcept>
#include <vector>

#include "UIFrame.h"
#include "UIButton.h"

#include "../Platform/IO/Inputs.h"


class UIRenderer
{
	static std::vector<UIElement*> m_renderList;
	static Mouse* attachedMouse;



public:

	static void attachMouse(Mouse* mouse)
	{
		attachedMouse = mouse;
	}

	static bool Button(int x, int y, int w, int h)
	{
		if (!attachedMouse) throw std::runtime_error("You have not attached a mouse to the ui system");
		UIButton button{};
		button.x = x;
		button.y = y;
		button.width = w;
		button.height = h;

		m_renderList.push_back(&button);
		return button.isClicked(attachedMouse->getPosX(), attachedMouse->getPosY(), attachedMouse->isLeftPressed());

	}


	static void clear()
	{
		m_renderList.clear();

	}
	static void renderUI()
	{

		for (auto* elem : m_renderList)
		{
			elem->render();
		}

		clear();

	}


	
};
