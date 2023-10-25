#include "Cloporte.h"

#include "Renderer.h"
#include "../../Graphics/abstraction/Camera.h"
#include "../../Platform/WindowsEngine.h"
#include "World/Managers/MeshManager.h"

extern std::unique_ptr<Mouse> wMouse;
extern std::unique_ptr<Keyboard> wKbd;

Cloporte::Cloporte() 
{

	m_cameras.first.setProjection<PerspectiveProjection>(PerspectiveProjection{});
	m_cameras.second.setProjection<PerspectiveProjection>(PerspectiveProjection{});

	//m_mesh = MeshManager::loadMeshFromFile("res/mesh/Game/boule.obj");


}

void Cloporte::handleInputs() 
{

	Vec delta{};

	Keyboard::Event e = wKbd->readKey();

	if (wKbd->isKeyPressed(VK_SPACE)) {
	} 

}
void Cloporte::draw(Camera& cam)
{
	//gPassFx.bindTexture("tex", bb.getTexture());
	//Renderer::renderMesh(cam, bunny, gPassFx);
}
void Cloporte::update(float deltaTime)
{

}


void Cloporte::handleKeyboardInputs() 
{

}