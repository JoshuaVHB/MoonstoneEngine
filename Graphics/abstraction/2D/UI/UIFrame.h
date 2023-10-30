#pragma once

#include "UIElement.h"

class UIFrame : public UIElement	
{
private:

	int x, y;
	int width, height;

public:

	void render();



};
