#include "UIRenderer.h"


Mouse* UIRenderer::attachedMouse = nullptr;
std::vector<UIElement*> UIRenderer::m_renderList = std::vector<UIElement*>{};