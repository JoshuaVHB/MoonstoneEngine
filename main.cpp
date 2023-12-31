#include <iostream>

#include "Utils/vendor/stdafx.h"
#include "Platform/WindowsEngine.h"
#include "Graphics/Renderer.h"

#include "Graphics/Rendering_impl/direct3D11_impl.h"
#include "Graphics/Rendering_impl/Rendering_impl.h"


#include "Graphics/World/Cube.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "src/Scenes/AllScenes.h"

#include "Physics/PhysicEngine.h"
#include "Physics/physx_Impl/physx_impl.h"


void initImgui(HWND hWnd, d3d11_graphics& gfx) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      // Enable Gamepad Controls

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(gfx.getDevice(), gfx.getImmediateContext());
	WindowsEngine& rMoteur = WindowsEngine::getInstance();
	rMoteur.imguiInit = true;
}

void shutdownImgui(){
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

#ifdef NDEBUG
int APIENTRY _tWinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

#else
int main() {
#endif
	try
	{

		WindowsEngine& rMoteur = WindowsEngine::getInstance();

		PhysicsEngine::setImplementation<Physx_Impl>();
		PhysicsEngine::onInit();


		//rMoteur.SetWindowsAppInstance(hInstance);
		rMoteur.init();

		d3d11_graphics& gfx = rMoteur.getGraphics();
		initImgui(rMoteur.getHwnd(), gfx);
		Renderer::setImplementation<direct3D11_impl>();

		//SceneManager::registerScene<TestScene>("TEST");
		//SceneManager::registerScene<SceneDeferred>("Deferred");
		//SceneManager::registerScene<InGame>("InGame");
		//SceneManager::registerScene<UITestScene>("UITestScene");
		SceneManager::registerScene<MainMenu>("MainMenu");
		SceneManager::registerScene<GameScene>("Game");
		SceneManager::switchToScene(0);


		rMoteur.run();
		
		PhysicsEngine::cleanupPhysics(false);

		shutdownImgui();
		return 1;
	}

	catch (const std::exception& E)
	{
		const int BufferSize = 1024;
		wchar_t message[BufferSize];

		size_t numCharacterConverted;
		mbstowcs_s(&numCharacterConverted, message, E.what(), BufferSize - 1);

		MessageBox(NULL, message, L"Erreur", MB_ICONWARNING);

		return 99;
	}

	catch (int err)/*int err*/
	{

		wchar_t szErrMsg[MAX_LOADSTRING];// Un message d'erreur selon le code
		//::LoadString(hInstance, err, szErrMsg, MAX_LOADSTRING);
		::MessageBox(nullptr, szErrMsg, L"Erreur", MB_ICONWARNING);
		(void)err;
		return (int)99; // POURQUOI 99???
	}


}
