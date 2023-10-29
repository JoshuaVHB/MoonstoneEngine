#include "Cloporte.h"

#include "CameraController.h"
#include "Renderer.h"
#include "../../Graphics/abstraction/Camera.h"
#include "../../Platform/WindowsEngine.h"
#include "World/Managers/MeshManager.h"

extern std::unique_ptr<Mouse> wMouse;
extern std::unique_ptr<Keyboard> wKbd;

CameraController controller;

Cloporte::Cloporte()
	: m_mesh{MeshManager::loadMeshFromFile("res/mesh/Game/boule.obj")}
	, m_speed(0), maxVelocity(3.f)
	, accelerationFactor(0.05f), friction(0.1f)
	, m_position{0, 0, 0, 0}
	, m_forward {0, 0, 1 , 0}
	, currentVelocity{0}

{
	m_currentCam = &m_thirdPerson;

	m_thirdPerson.setProjection<PerspectiveProjection>(PerspectiveProjection{});
	m_firstPerson.setProjection<PerspectiveProjection>(PerspectiveProjection{});
	m_boundingSphere = BoundingSphere(m_position, XMVectorGetX(m_mesh.getTransform().getScale()));
}

void Cloporte::handleInputs() 
{

	Vec delta{};

	Keyboard::Event e = wKbd->readKey();

	

}
void Cloporte::draw(Camera& cam)
{

}
void Cloporte::update(float deltaTime)
{
	//computeForward();
	handleKeyboardInputs(deltaTime);
	updatePosition(deltaTime);
	m_boundingSphere.origin = m_position;
	//getCurrentCamera().setPosition(
	//	XMVectorAdd(m_position,{ -20 * XMVectorGetX(m_forward) ,5,-20* XMVectorGetZ(m_forward)}));
	CameraController::computeThirdPersonPosition(*this, *m_currentCam);
	getCurrentCamera().updateCam();

}

void Cloporte::updatePosition(float deltaTime)
{

	m_position = XMVectorAdd(m_position, m_forward * currentVelocity * deltaTime * 100);
	currentVelocity *= 0.9 ;

	m_mesh.getTransform().setPosition(m_position);
}


void Cloporte::handleKeyboardInputs(float deltaTime)
{
	DirectX::XMVECTOR m_positionDelta{};
	if (wKbd->isKeyPressed(Keyboard::letterCodeFromChar('z')) || wKbd->isKeyPressed(Keyboard::letterCodeFromChar('w')))
	{
		//float dx = std::clamp(XMVectorGetX(m_velocity) + accelerationFactor * XMVectorGetX(m_forward), 0.f, maxVelocity);
		//float dz = std::clamp(XMVectorGetZ(m_velocity) + accelerationFactor * XMVectorGetZ(m_forward), 0.f, maxVelocity);

		currentVelocity = std::clamp( currentVelocity + accelerationFactor, 0.f, maxVelocity);

	}

	if (wKbd->isKeyPressed(Keyboard::letterCodeFromChar('s')) )
	{
		currentVelocity = std::clamp( currentVelocity - accelerationFactor, 0.f, maxVelocity);
		//float dx = std::clamp(XMVectorGetX(m_velocity) + accelerationFactor * -XMVectorGetX(m_forward), 0.f, maxVelocity);
		//float dz = std::clamp(XMVectorGetZ(m_velocity) + accelerationFactor * -XMVectorGetZ(m_forward), 0.f, maxVelocity);
		//m_velocity = XMVectorSetX(m_velocity, dx);
		//m_velocity = XMVectorSetZ(m_velocity, dz);
	}

	if (wKbd->isKeyPressed(Keyboard::letterCodeFromChar('q')) || wKbd->isKeyPressed(Keyboard::letterCodeFromChar('a')))
	{

		m_forward = XMVector3Rotate(m_forward, XMQuaternionRotationAxis({ 0,1,0 }, -0.05f));

	}

	if (wKbd->isKeyPressed(Keyboard::letterCodeFromChar('d')) )
	{

		m_forward = XMVector3Rotate(m_forward, XMQuaternionRotationAxis({ 0.f,1.f,0.f }, 0.05f));

	}

}