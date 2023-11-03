#include "Cloporte.h"

#include "CameraController.h"
#include "Renderer.h"
#include "../../Graphics/abstraction/Camera.h"
#include "../../Platform/WindowsEngine.h"
#include "World/Managers/MeshManager.h"

#include "../../Physics/PhysicEngine.h"
#include "../../Physics/physx_impl/physx_shape.h"
extern std::unique_ptr<Mouse> wMouse;
extern std::unique_ptr<Keyboard> wKbd;

CameraController controller;

Cloporte::Cloporte()
	: m_object{ PhysicsEngine::FilterGroup::ePlayer, PhysicsEngine::FilterGroup::eAll }
	, m_speed(0), maxVelocity(3.f)
	, accelerationFactor(0.05f), friction(0.1f)
	, m_position{0, 0, 0, 0}
	, m_forward {0, 0, 1 , 0}
	, currentVelocity{0}

{
	m_currentCam = &m_thirdPerson;

	m_thirdPerson.setProjection<PerspectiveProjection>(PerspectiveProjection{});
	m_firstPerson.setProjection<PerspectiveProjection>(PerspectiveProjection{});

	static Mesh m_mesh{ MeshManager::loadMeshFromFile("res/mesh/Game/boule.obj") };
	m_object.setMesh(std::move(&m_mesh));
	m_object.addShape(physx_shape::getBall(XMVectorGetX(m_object.getTransform().getScale())));
	m_boundingSphere = BoundingSphere(m_position, XMVectorGetX(m_object.getTransform().getScale()));
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
	
	m_object.getTransform().getPosition() = XMVectorAdd(m_object.getTransform().getPosition(), m_forward * currentVelocity * deltaTime * 100);
	m_position = m_object.getTransform().getPosition();
	currentVelocity *= 0.9 ;
	m_object.updateTransform();

}


void Cloporte::handleKeyboardInputs(float deltaTime)
{
	DirectX::XMVECTOR m_positionDelta{};
	if (wKbd->isKeyPressed(Keyboard::letterCodeFromChar('z')) || wKbd->isKeyPressed(Keyboard::letterCodeFromChar('w')))
	{
		//float dx = std::clamp(XMVectorGetX(m_velocity) + accelerationFactor * XMVectorGetX(m_forward), 0.f, maxVelocity);
		//float dz = std::clamp(XMVectorGetZ(m_velocity) + accelerationFactor * XMVectorGetZ(m_forward), 0.f, maxVelocity);

		PhysicsEngine::fVec3 linearVelocity = m_object.getLinearValocity();
		//linearVelocity.normalize();
		PhysicsEngine::fVec3 forward { XMVectorGetX(m_forward), XMVectorGetY(m_forward), XMVectorGetZ(m_forward) };
		m_object.addForce(forward * 100);

	}

	if (wKbd->isKeyPressed(Keyboard::letterCodeFromChar('s')) )
	{
		PhysicsEngine::fVec3 linearVelocity = m_object.getLinearValocity();
		//linearVelocity.normalize();
		PhysicsEngine::fVec3 forward { XMVectorGetX(m_forward), XMVectorGetY(m_forward), XMVectorGetZ(m_forward) };
		m_object.addForce(-forward * accelerationFactor);
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