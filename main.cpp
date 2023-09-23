#include <iostream>

#include "Utils/vendor/stdafx.h"
#include "Platform/WindowsEngine.h"
#include "Graphics/Renderer.h"

#include "Graphics/Rendering_impl/direct3D11_impl.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

using namespace MS;

void initImgui(HWND hWnd, Graphics& gfx) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(gfx.getDevice(), gfx.getImmediateContext());
}

void shutdownImgui(){
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

int APIENTRY _tWinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	try
	{
		WindowsEngine& rMoteur = WindowsEngine::getInstance();



		rMoteur.SetWindowsAppInstance(hInstance);
		rMoteur.init();

		Renderer::setImplementation<direct3D11_impl>();
		Graphics& gfx = rMoteur.getGraphics();

		initImgui(rMoteur.getHwnd(), gfx);
		rMoteur.run();
		

		shutdownImgui();
		return 1;
	}

	catch (const std::exception& E)
	{
		const int BufferSize = 128;
		wchar_t message[BufferSize];

		size_t numCharacterConverted;
		mbstowcs_s(&numCharacterConverted, message, E.what(), BufferSize - 1);

		MessageBox(NULL, message, L"Erreur", MB_ICONWARNING);

		return 99;
	}

	catch (int err)
	{
		wchar_t szErrMsg[MAX_LOADSTRING];// Un message d'erreur selon le code
		::LoadString(hInstance, err, szErrMsg, MAX_LOADSTRING);
		::MessageBox(nullptr, szErrMsg, L"Erreur", MB_ICONWARNING);
		return (int)99; // POURQUOI 99???
	}


}
