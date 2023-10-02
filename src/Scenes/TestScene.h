#pragma once

#include "Scene.h"

#include "../../Graphics/World/Cube.h"
#include "../../Graphics/abstraction/Camera.h"

#include "../../Platform/IO/Inputs.h"
#include "../../Platform/WindowsEngine.h"
#include <iostream>
#include <memory>


extern std::unique_ptr<Keyboard> wKbd;
extern std::unique_ptr<Mouse> wMouse;


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

};

inline std::ostream& operator<<(std::ostream& os, const Vec2& v) {
	os << "(" << (v.x) << "," << v.y << ")";
	return os;
}

class TestScene : public Scene {

private:

	Cube c;
	
	Camera camera;
	Vec delta = camera.getPosition();

	float dt = 0;
	float elapsed = 0;

	bool render = true;
	bool tmp = true;
	char text[500] = {};

	Vec2 mousePose;
	Vec2 mouseDelta;

	std::pair<int, int> winSize{};

public:

	TestScene() 
	{
		camera.setProjection<PerspectiveProjection>(PerspectiveProjection());	
		winSize = WindowsEngine::getInstance().getGraphics().getWinSize();

	}


	virtual void onUpdate(float deltaTime) override
	{
		dt = deltaTime;
		elapsed += deltaTime;

		Mouse::Event me = wMouse->read();

		if (me.isValid()) {

			mouseDelta = Vec2(me.getPos()) - mousePose;
			mousePose = Vec2(me.getPos());

			Vec2 rotationMotion = (mouseDelta / Vec2(winSize)) * 3.1415926f;
			std::cout << mousePose << "|" << rotationMotion << std::endl;
			float yaw = camera.getYaw() + rotationMotion.x;

			float pitch = max(-3.1415926f * .499f,
				min(3.1415926f * .499f,
					camera.getPitch() + rotationMotion.y
				)
			);
			std::cout << pitch << " ---- " << yaw << std::endl;
			camera.setYaw(yaw);
			camera.setPitch(pitch);
		}
		camera.setPosition(delta);
		camera.updateCam(deltaTime);

		Keyboard::Event e = wKbd->readKey();
		if (e.isPress() && e.getCode()==VK_SPACE)
			render = !render;

		if (wKbd->isKeyPressed(Keyboard::letterCodeFromChar('q'))) {
			delta.vector4_f32[0] -= 0.5f;
		}

		if (wKbd->isKeyPressed(Keyboard::letterCodeFromChar('d'))) {
			delta.vector4_f32[0] += 0.5f;
		}

		if (wKbd->isKeyPressed(Keyboard::letterCodeFromChar('z'))) {
			delta.vector4_f32[2] += 0.5f;
		}

		if (wKbd->isKeyPressed(Keyboard::letterCodeFromChar('s'))) {
			delta.vector4_f32[2] -= 0.5f;
		}
	}

	virtual void onRender() override {
	
		Renderer::clearScreen(1.f,1.f,0.f,1.f);
		if (render) Renderer::renderMesh(camera, dt);
	}
	virtual void onImGuiRender()override {
		ImGui::Begin("Debug");

		ImGui::Text(std::to_string(render).c_str());
		ImGui::Text(std::to_string(wKbd->isKeyPressed(VK_SPACE)).c_str());
		ImGui::Text(std::to_string(tmp).c_str());
		ImGui::DragFloat4("delta", &delta.vector4_f32[0]);
		ImGui::InputText("coucou", text, 500);
		ImGui::End();

	}


};
