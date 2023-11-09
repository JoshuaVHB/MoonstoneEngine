#pragma once

#include "Scene.h"
#include "../../Graphics/World/Player.h"
#include "../../Graphics/World/WorldRendering/Skybox.h"

#include "../../Platform/WindowsEngine.h"

#include "../../Graphics/World/Managers/MeshManager.h"

#include "../../Utils/Transform.h"
#include "abstraction/DeferredRenderer.h"
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
		: m_renderer{}
	{
		bunny = MeshManager::loadMeshFromFile("res/mesh/bunny.obj");

		MeshManager::feed(new Mesh(std::move(bunny)) );

		Renderer::setBackbufferToDefault();
	}

	void renderFn()
	{
		static auto deferredRenderLambda = [&](Camera& cam, const Mesh& mesh)	{	m_renderer.renderMesh(cam, mesh);	};
		MeshManager::render(m_player.getCamera(), deferredRenderLambda);
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
		//bunny.getTransform().showControlWindow();
		ImGui::End();
		m_player.onImGuiRender();
		Renderer::showImGuiDebugData();
		m_renderer.showDebugWindow();
	}

	~SceneDeferred()
	{
		
		MeshManager::clear();
	}

};
