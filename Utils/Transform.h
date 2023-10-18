#pragma once

#include "imgui.h"
#include <DirectXMath.h>

using namespace DirectX;

static int debug_count = 0;

class Transform
{
private:


	XMVECTOR translation{0,0,0,0};
	XMVECTOR scale{1,1,1,1};

	float angles[3] = {};
	float position[3] = {};
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

	XMVECTOR getPosition() const noexcept {
		return XMVECTOR{ position[0], position[1], position[2] };
	}

	XMVECTOR getScale() const noexcept {
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
		std::string name = "Transform " + std::to_string(debug_count);
		if (ImGui::CollapsingHeader(name.c_str())) {
			ImGui::DragFloat3("Rotation", angles, 0.1f);
			if (ImGui::DragFloat3("Translation", position, 0.1f))
			{
				translation = { position[0], position[1], position[2] };

			}
			if (ImGui::DragFloat3("scale", _scale, 0.1F))
			{
				scale = { _scale[0], _scale[1], _scale[2] };
			}
		}
	}

};

