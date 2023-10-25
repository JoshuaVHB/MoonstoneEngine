#include "Scene.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"


std::vector<SceneBuilder> SceneManager::s_availableScenes{};
size_t SceneManager::s_activeSceneIndex		= -1;
Scene* SceneManager::s_activeScene			= nullptr;

void SceneManager::onRender() 
{
	s_activeScene->onRender();
}

void SceneManager::onUpdate(float deltaTime)
{
	s_activeScene->onUpdate(deltaTime);
}

void SceneManager::onImGuiRender()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    s_activeScene->onImGuiRender();

    ImGui::Begin("Scenes");
    for (size_t i = 0; i < s_availableScenes.size(); i++) {
        auto& [name, provider] = s_availableScenes[i];
        if (ImGui::Button(name.c_str()))
            switchToScene(i);
        if (s_activeSceneIndex == i) {
            ImGui::SameLine();
            ImGui::Text("< active");
        }
    }
    ImGui::End();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void SceneManager::registerScene(const std::string& name, const SceneBuildFn& provider)
{
    s_availableScenes.emplace_back(std::make_pair(name, provider));
}

void SceneManager::switchToScene(size_t sceneIndex)
{
    delete s_activeScene;
    auto& [name, provider] = s_availableScenes[sceneIndex];
    s_activeScene = provider();
    s_activeSceneIndex = sceneIndex;
}
