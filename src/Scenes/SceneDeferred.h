#pragma once

#include "Scene.h"
#include "../../Graphics/Rendering_impl/direct3D11_impl.h"

#include "../../Graphics/World/Cube.h"
#include "../../Graphics/abstraction/Camera.h"
#include "..\..\Graphics\abstraction\Effect.h"
#include "../../Graphics/abstraction/Framebuffer.h"
#include "../../Graphics/World/Mesh.h"
#include "../../Graphics/World/Cube.h"
#include "../../Graphics/World/Player.h"
#include "../../Graphics/World/WorldRendering/Skybox.h"

#include "../../Platform/IO/Inputs.h"
#include "../../Platform/WindowsEngine.h"
#include <iostream>
#include <memory>
#include <directXmath.h>

#include "../../Graphics/World/Material.h"
#include "../../Graphics/World/Frustum.h"
#include "../../Graphics/World/Managers/MeshManager.h"

#include "../../Utils/Transform.h"
#include "abstraction/DeferredRenderer.h"
#include "../../Graphics/abstraction/2D/Sprite.h"
#include "../../Graphics/abstraction/2D/TextRenderer.h"

#define DRAGFLOAT(flt) ImGui::DragFloat(#flt, &flt, 1,-100,100);



class SceneDeferred : public Scene {

private:

	float dt = 0, elapsed = 0;

	Mesh bunny;
	Skybox box;
	Player m_player;
	DeferredRenderer m_renderer;
	TextRenderer m_textRenderer;

public:

	SceneDeferred()
	{
		bunny = MeshManager::loadMeshFromFile("res/mesh/bunny.obj");
		Renderer::setBackbufferToDefault();

	}

	void renderFn()
	{
		m_renderer.renderMesh(m_player.getCamera(), bunny);
		m_renderer.renderSkybox(m_player.getCamera(), box);

	}


	virtual void onUpdate(float deltaTime) override
	{
		dt = deltaTime;
		elapsed += deltaTime;

		m_player.step(elapsed);
		m_renderer.update(m_player.getCamera());
		m_textRenderer.clear();

	}

	virtual void onRender() override {

		Renderer::clearScreen();
		m_renderer.clear();

		m_renderer.renderDeferred([&]() {renderFn(); }, m_player.getCamera());
		Renderer::setBackbufferToDefault();

		m_textRenderer.writeTextOnScreen(std::to_string(elapsed), -500, -200, 1);
		m_textRenderer.render();

	}
	virtual void onImGuiRender() override {
		ImGui::Begin("Debug");
		ImGui::Text("MY BELOVED TEST SCENE !!! THIS IS HIGHLY EXPERIMENTAL");
		bunny.getTransform().showControlWindow();

		ImGui::End();

		m_player.onImGuiRender();
		Renderer::showImGuiDebugData();
		m_renderer.showDebugWindow();
	}


};
