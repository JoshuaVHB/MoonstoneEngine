#pragma once

#include <DirectXMath.h>
#include <memory>
#include <concepts>
#include <iostream>
#include <algorithm>
#include <cstdlib>

using namespace DirectX;
using Mat4 = XMFLOAT4X4;
using Mat = XMMATRIX;
using Vec = XMVECTOR;

struct Vec2 {

	union {
		struct { float x, y; };
		float body[2];
	};

	Vec2() : body{} {}
	Vec2(float _x, float _y) : x(_x), y(_y) {}

	template<typename _type> requires (std::convertible_to<_type, float>)
		Vec2(const std::pair<_type, _type>& p) { x = p.first; y = p.second; }

	Vec2 operator+(const Vec2& other) { return Vec2{ x + other.x, y + other.y }; }
	Vec2 operator-(const Vec2& other) { return Vec2{ x - other.x, y - other.y }; }
	Vec2 operator/(const Vec2& other) { return Vec2{ x / other.x, y / other.y }; }
	Vec2 operator*(const Vec2& other) { return Vec2{ x * other.x, y * other.y }; }
	Vec2 operator*(float scalar) { return Vec2{ x * scalar, y * scalar }; }
	Vec2 operator/(float scalar) { float inv = 1.f / scalar; return Vec2{ x,y }*inv; }

};

inline std::ostream& operator<<(std::ostream& os, const Vec2& v) {
	os << "(" << (v.x) << "," << v.y << ")";
	return os;
}


struct Projection {

	Mat projMat;
	virtual Mat getProjMatrix() const noexcept = 0;
protected:

};

struct PerspectiveProjection : Projection
{
	float aspectRatio = 2.0f;
	float FOV = XM_PI / 4.f;

	float znear = 0.10f, zfar = 1000.F;

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

	struct _angles {
		float yaw = 0.F;
		float pitch = 0.F;
		float roll = 0.f;

		XMVECTOR toVec() const { return { -pitch ,-yaw, -roll }; }
	} m_angles ;

	std::unique_ptr<Projection> m_projection;

	Vec m_position = {0,0,-10.f,1.0f};
	Vec m_target{0,0,0,1};
	Vec m_UP{0,1,0,1};
	Vec m_left{1,0,0,1};

	/*
	float m_pitch = 0.f;
	float m_yaw = 0.f;
	*/

	float t = 0;
	float m_rotationSpeed =1.f;



public:

	void updateCam() 
	{
		computeVPMatrix();
	}

	void move(Vec delta) {
		m_position += delta;
	}

	void setPosition(Vec pos) {
		m_position = pos;
	}

	void computeVPMatrix() {

		computeViewMatrix();
		m_viewProjMatrix =  m_viewMatrix * m_projection->getProjMatrix();

	}
	
	void computeViewMatrix() {

		m_viewMatrix = XMMatrixIdentity();
		m_viewMatrix = m_viewMatrix * XMMatrixTranslationFromVector(-m_position);

		m_viewMatrix = m_viewMatrix * XMMatrixRotationAxis({ 0,1,0,1 }, -m_angles.yaw);
		m_viewMatrix = m_viewMatrix * XMMatrixRotationAxis({ 1,0,0,1 }, -m_angles.pitch);
		//m_viewMatrix = m_viewMatrix * XMMatrixRotationAxis({ 0,0,1,1 }, -m_angles.roll);

	}

	void rotate(float dx = 0.0F, float dy = 0.0F, float dz = 0.0F) 
	{
		m_angles.yaw += dx * m_rotationSpeed;
		m_angles.pitch = std::clamp(m_angles.pitch + dy, -DirectX::XM_PI * .499f, DirectX::XM_PI * .499f);
		

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

	XMVECTOR getAngles() const noexcept { return m_angles.toVec(); }

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
	
			double cy = cos(m_angles.yaw), sy = sin(m_angles.yaw);
			double cp = cos(m_angles.pitch), sp = sin(m_angles.pitch);
			return { - static_cast<float>(sy * cp), static_cast<float>(sp), static_cast<float> ( - cy * cp)};
	}

	Vec getHorizontalDir() const noexcept {

		return XMVector4Normalize(XMVector3Cross(getForwardDir(), m_UP));
	}

	Vec getForwardDir() const noexcept {
		return XMVector4Normalize(getForward());
	}



	void setYaw(float yaw) { m_angles.yaw = yaw; }
	void setPitch(float pitch) { m_angles.pitch = pitch; }
	[[nodiscard]] float getPitch() const noexcept {return m_angles.pitch ; }
	[[nodiscard]] float getYaw() const noexcept { return m_angles.yaw; }

	[[nodiscard]] float& getRawYaw() { return m_angles.yaw; }
	[[nodiscard]] float& getRawPitch() { return m_angles.pitch; }

};

