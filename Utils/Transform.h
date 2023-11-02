#pragma once

#include "imgui.h"
#define _XM_NO_INTRINSICS_
#include <DirectXMath.h>

using namespace DirectX;

static int debug_count = 0;

class Transform
{
private:

	XMVECTOR translation{0,0,0,0};
	XMVECTOR scale{1,1,1,1};
	//XMVECTOR rotation{0,0,0};

	float angles[3] = {};
	float position[3] = {00, 20, 50};
	float _scale[3] = {1,1,1};

public:
	Transform()
	{
		debug_count++;
	}

	~Transform()
	{
		debug_count--;
	}

	XMVECTOR& getPosition() noexcept {
		return translation;
	}

	void setPosition(XMVECTOR pos)
	{
		translation = pos;
		position[0] = XMVectorGetX(translation);
		position[1] = XMVectorGetY(translation);
		position[2] = XMVectorGetZ(translation);
	}


	void setRotation(XMVECTOR rot)
	{
		//rotation = rot;
		angles[0] = XMVectorGetX(rot);
		angles[1] = XMVectorGetY(rot);
		angles[2] = XMVectorGetZ(rot);
	}

	void setScale(XMVECTOR sc)
	{
		scale = sc;
		_scale[0] = XMVectorGetX(scale);
		_scale[1] = XMVectorGetY(scale);
		_scale[2] = XMVectorGetZ(scale);
	}

	const XMVECTOR& getPosition() const noexcept {
		return translation;
	}

	XMVECTOR getScale() noexcept {
		return XMVECTOR{ _scale[0], _scale[1], _scale[2] };
	}

	XMMATRIX getTransformationMatrix() const noexcept
	{
		XMVECTOR origin = { 0,0,0,1 };
		auto rotation = XMQuaternionRotationRollPitchYaw(angles[0], angles[1], angles[2]);
		return XMMatrixTransformation(origin, rotation, scale, origin, rotation, translation);
	}

	void showControlWindow()
	{
		std::string name = "Transform #" + std::to_string(debug_count);
		if (ImGui::CollapsingHeader(name.c_str())) {
			if (ImGui::DragFloat3("Rotation", angles, 0.1f))
			{
				//rotation = {angles[0], angles[1], angles[2]};
			}
			if (ImGui::DragFloat3("Translation", &position[0], 0.1f))
			{
				translation = { position[0] ,position[1] ,position[2] };
			}

			if (ImGui::DragFloat3("scale", _scale, 0.1F))
			{
				scale = { _scale[0], _scale[1], _scale[2] };
			}
		}
	}

	
	void setTranslation(float x, float y, float z) {
		translation = DirectX::XMVectorSet(x, y, z, 0);
	}
	
	void setAngles(float x, float y, float z) {
		angles[0] = x;
		angles[1] = y;
		angles[2] = z;
	}
};

