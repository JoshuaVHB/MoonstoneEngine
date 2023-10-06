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


void initImgui(HWND hWnd, d3d11_graphics& gfx) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

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

//int APIENTRY _tWinMain(
//	HINSTANCE hInstance,
//	HINSTANCE hPrevInstance,
//	LPTSTR    lpCmdLine,
//	int       nCmdShow)
//{
//	UNREFERENCED_PARAMETER(hPrevInstance);
//	UNREFERENCED_PARAMETER(lpCmdLine);
//	UNREFERENCED_PARAMETER(nCmdShow);
int main() {
	try
	{
		WindowsEngine& rMoteur = WindowsEngine::getInstance();
		


		//rMoteur.SetWindowsAppInstance(hInstance);
		rMoteur.init();

		Renderer::setImplementation<direct3D11_impl>();
		d3d11_graphics& gfx = rMoteur.getGraphics();
		initImgui(rMoteur.getHwnd(), gfx);

		SceneManager::registerScene<TestScene>("TEST");
		SceneManager::registerScene<Scene2>("Scene2");
		SceneManager::registerScene<Sponza>("Sponza");
		SceneManager::switchToScene(0);


		rMoteur.run();
		

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
		///::LoadString(hInstance, err, szErrMsg, MAX_LOADSTRING);
		::MessageBox(nullptr, szErrMsg, L"Erreur", MB_ICONWARNING);
		(void)err;
		return (int)99; // POURQUOI 99???
	}


}
