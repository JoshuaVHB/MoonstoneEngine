#pragma once

#include <DirectXMath.h>
#include <memory>
#include <concepts>

using namespace DirectX;
using Mat4 = XMFLOAT4X4;
using Mat = XMMATRIX;
using Vec = XMVECTOR;

struct Projection {

	Mat projMat;
	virtual Mat getProjMatrix() const noexcept = 0;
protected:

};

struct PerspectiveProjection : Projection
{
	float aspectRatio = 2.0f;
	float FOV = XM_PI / 4;

	float znear = 2.0f, zfar = 20.F;

	PerspectiveProjection() {
		projMat = XMMatrixPerspectiveFovLH(FOV, aspectRatio, znear, zfar);
	}

	virtual Mat getProjMatrix() const noexcept override {

		return  XMMatrixPerspectiveFovLH(FOV, aspectRatio, znear, zfar);
	}



};


struct OrthographicProjection : public Projection 
{
	float left, right;
	float top, bot;
	float znear, zfar;

	Mat projMat;

	OrthographicProjection(float l, float r, float t, float b, float zn, float zf)
	{
		left = l;
		right = r;
		top = t;
		bot = b;
		znear = zn;
		zfar = zf;

		projMat = XMMatrixOrthographicLH(left - right, top - bot, znear, zfar);

	}

	virtual Mat getProjMatrix() const noexcept override
	{
		return projMat;
	}
};

class Camera
{
private:

	Mat m_viewProjMatrix;
	Mat m_viewMatrix;

	std::unique_ptr<Projection> m_projection;

	Vec m_position = {0,0,-10.f,1.0f};
	Vec m_target{0,0,0,1};
	Vec m_UP{0,1,0,1};

	float m_pitch = 0.f;
	float m_yaw = 0.f;

public:



	void computeVPMatrix() {
		
		/*
		computeViewMatrix();
		m_viewMatrix = XMMatrixLookAtLH(XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f),
			XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
			XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f));

		*/
		lookAt();
		m_viewProjMatrix =  m_viewMatrix * m_projection->getProjMatrix();

	}
	
	void computeViewMatrix() {

		m_viewMatrix = XMMatrixIdentity() * XMMatrixRotationAxis({ 1,0,0,0 }, -m_pitch);
		m_viewMatrix = m_viewMatrix * XMMatrixRotationAxis({ 0,1,0,0 }, -m_yaw);
		m_viewMatrix = m_viewMatrix * XMMatrixTranslationFromVector(-m_position);
	}


	void lookAt(Vec target) 
	{
		m_target = target;
		m_viewMatrix = XMMatrixLookAtLH(m_position, m_target, m_UP);
	}

	void lookAt()
	{
		m_viewMatrix = XMMatrixLookAtLH(m_position, m_target, m_UP);
	}


	Mat getVPMatrix() {
		computeVPMatrix();
		return m_viewProjMatrix;
	}

	Mat getProjMatrix() const {
		return m_projection->getProjMatrix();
	}
	
	template<class _proj=PerspectiveProjection>
	requires (std::derived_from<_proj, Projection>)
	void setProjection(const _proj& proj)
	{
		m_projection = std::make_unique<_proj>(proj);
	}

	Vec getPosition() const noexcept { return m_position; }

	Vec getForward() const noexcept { 	
	
			float cy = cos(m_yaw), sy = sin(m_yaw);
			float cp = cos(m_pitch), sp = sin(m_pitch);
			return { -sy * cp, sp, -cy * cp };
	}

};

