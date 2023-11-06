#pragma once

#include <stdexcept>
#include <vector>

#include "UIFrame.h"
#include "UIButton.h"

#include "../Platform/IO/Inputs.h"
#include "abstraction/2D/Renderer2D.h"



class UIRenderer
{
	static std::vector<std::unique_ptr<UIElement>> m_renderList;
	static Mouse* attachedMouse;

	static Renderer2D* m_renderer;


public:

	static void attachMouse(Mouse* mouse)
	{
		if (!attachedMouse) m_renderer = new Renderer2D; // worst line ever, todo chgange this
		attachedMouse = mouse;
	}

	static bool Button(int x, int y, int w, int h)
	{
		if (!attachedMouse) throw std::runtime_error("You have not attached a mouse to the ui system");
		std::unique_ptr<UIButton> button = std::make_unique<UIButton>();
		button->x = x;
		button->y = y;
		button->width = w;
		button->height = h;
		button->hovered = button->isHovered(attachedMouse->getPosX(), attachedMouse->getPosY());
		bool res = button->isClicked(attachedMouse->getPosX(), attachedMouse->getPosY(), attachedMouse->isLeftPressed());
		m_renderList.push_back(std::move(button));
		return res;

	}


	static void clear()
	{
		m_renderList.clear();

	}
	static void renderUI()
	{
		m_renderer->BeginBatch();
		for (auto&& elem : m_renderList)
		{
			elem->render(*m_renderer);
		}
		m_renderer->EndBatch();
		m_renderer->Flush();
		
		clear();
		m_renderer->clear();

	}


	
};
