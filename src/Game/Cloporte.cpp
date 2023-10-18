#include "Cloporte.h"
#include "../../Graphics/abstraction/Camera.h"
#include "../../Platform/WindowsEngine.h"

extern std::unique_ptr<Mouse> wMouse;
extern std::unique_ptr<Keyboard> wKbd;

Cloporte::Cloporte() 
{

	m_cameras.first.setProjection<PerspectiveProjection>(PerspectiveProjection{});
	m_cameras.second.setProjection<PerspectiveProjection>(PerspectiveProjection{});

}

void Cloporte::handleInputs() 
{

	Vec delta{};

	Keyboard::Event e = wKbd->readKey();

	if (wKbd->isKeyPressed(VK_SPACE)) {
	} 

}
void Cloporte::draw() {

}
void Cloporte::update(float deltaTime) {

}


void Cloporte::handleKeyboardInputs() 
{

}