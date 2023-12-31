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
	, maxVelocity(10.0f)
	, accelerationFactor(0.05f)
	, m_position{0, 0, 0, 0}
	, m_forward {0, 0, 1 , 0}
	, m_groundDir{0, 1, 0, 0}
	, currentVelocity{0}

{
	m_currentCam = &m_thirdPerson;

	m_thirdPerson.setProjection<PerspectiveProjection>(PerspectiveProjection{});
	m_firstPerson.setProjection<PerspectiveProjection>(PerspectiveProjection{});

	static Mesh m_mesh{ MeshManager::loadMeshFromFile("res/mesh/Game/boule.obj") };
	m_object.setMesh(std::move(&m_mesh));
	m_object.addShape(physx_shape::getBall(XMVectorGetX(m_object.getTransform().getScale())));
	m_boundingSphere = BoundingSphere(m_position, XMVectorGetX(m_object.getTransform().getScale()));

	//Set max velocity
	//m_object.setMaxLinearVelocity(maxVelocity);
	m_object.setMaxAngularVelocity(100.0f);

	//Set Velocity
	m_object.setLinearVelocity({ 0.5f,0.5f,0.5f });
	m_object.setAngularVelocity({ 0.5f,0.5f,0.5f });
	m_object.setMass(3.f);
}


void Cloporte::switchView() noexcept
{
	if (m_currentCam == &m_thirdPerson) m_currentCam = &m_firstPerson;
	else m_currentCam = &m_thirdPerson;
}

void Cloporte::update(float deltaTime)
{

	if( m_handleInputs ) handleKeyboardInputs(deltaTime);
	updatePosition(deltaTime);
	m_boundingSphere.origin = m_position;
	if (m_handleInputs) CameraController::computeThirdPersonPosition(*this, m_thirdPerson);
	if (m_handleInputs) CameraController::computeFirstPersonPosition(*this, m_firstPerson);

	getCurrentCamera().updateCam();

}

void Cloporte::updatePosition(float deltaTime)
{
	
	m_object.getTransform().getPosition() = XMVectorAdd(m_object.getTransform().getPosition(), m_forward * currentVelocity * deltaTime);
	m_position = m_object.getTransform().getPosition();
	currentVelocity *= 0.9f ;
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
		physx::PxVec3 rotationAxis = forward.cross(m_object.getPosition());

		m_object.addTorque(-rotationAxis * 10);
		m_object.addForce(forward * 10 );

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
	// stupid workaround
	static bool tmp = false;
	if (wKbd->isKeyPressed(VK_SPACE))
	{
		if (!tmp)
		{			
			switchView();
			tmp = true;
		}
	}

	if (!wKbd->isKeyPressed(VK_SPACE))
	{
		tmp = false;

	}

}


void Cloporte::setTranslation(float x, float y, float z)
{
	m_object.clearForce();
	m_object.clearTorque();
	m_object.setTranslation(x, y, z);
	m_object.getTransform().setTranslation(x, y, z);
	m_forward = { 0, 0, 1 , 0 };
	m_groundDir = { 0, 1, 0, 0 };
	currentVelocity = { 0 };
	m_object.setLinearVelocity({ 0,0,0 });
	m_object.setAngularVelocity({ 0,0,0 });
	m_position = DirectX::XMVECTOR{ x,y,z, 1 };

	auto pos = m_object.getTransform().getPosition();

}
void Cloporte::setPosition(float x, float y, float z) {
	m_object.clearForce();
	m_object.clearTorque();
	m_object.getTransform().setTranslation(x, y, z);
	m_object.majTransformPhysics();
	m_position = DirectX::XMVECTOR{ x,y,z, 1 };
	auto pos = m_object.getTransform().getPosition();

}