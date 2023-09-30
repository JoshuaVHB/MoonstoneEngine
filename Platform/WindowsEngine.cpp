#include "../Utils/vendor/stdafx.h"
#include "../resource.h"
#include "WindowsEngine.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <memory>
#include <iostream>



std::unique_ptr<Keyboard> wKbd = std::make_unique<Keyboard>();

namespace MS
{

HINSTANCE WindowsEngine::hAppInstance;	// handle Windows de l'instance actuelle de l'application

//   FONCTION : SetWindowsAppInstance(HANDLE, int)
//
//   BUT : Prend en note le handle de l'instance
//
void WindowsEngine::SetWindowsAppInstance(HINSTANCE hInstance)
{
	hAppInstance = hInstance; // Stocke le handle d'instance de l'application, plusieurs fonctions spécifiques en auront besoin
}

//   FONCTION : InitAppInstance()
//
//   BUT :  Effectue des opérations d'initialisation de l'application
//
//   COMMENTAIRES :
//
//        Dans cette fonction, nous enregistrons le handle de l'instance dans une variable globale, puis
//        créons et affichons la fenêtre principale du programme.
//
bool WindowsEngine::InitAppInstance()
{
	TCHAR szTitle[MAX_LOADSTRING];					// Le texte de la barre de titre

	// Initialise les chaînes globales
	LoadString(hAppInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hAppInstance, IDC_PETITMOTEUR3D, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hAppInstance);

	hMainWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hAppInstance, NULL);

	if (!hMainWnd)
	{
		return false;
	}

	hAccelTable = LoadAccelerators(hAppInstance, MAKEINTRESOURCE(IDC_PETITMOTEUR3D));

	return true;
}

Graphics* WindowsEngine::createRenderingDevicePlatform(GRAPHICS_MODE cdsMode) {
	m_graphics = std::make_unique<Graphics>(hMainWnd, cdsMode);
	return m_graphics.get();
}

Timer::value_type WindowsEngine::getTimePlatform() const
{
	return static_cast<Timer::value_type>(m_clock.getTimeCount());
}
Timer::value_type WindowsEngine::getTimeIntervalInSeconds(
	Timer::count_type start,
	Timer::count_type stop) const
{
	return m_clock.getTimeBetweenCounts(start, stop);
}


//
//  FONCTION : MyRegisterClass()
//
//  BUT : inscrit la classe de fenêtre.
//
//  COMMENTAIRES :
//
//    Cette fonction et son utilisation sont nécessaires uniquement si vous souhaitez que ce code
//    soit compatible avec les systèmes Win32 avant la fonction 'RegisterClassEx'
//    qui a été ajoutée à Windows 95. Il est important d'appeler cette fonction
//    afin que l'application dispose des petites icônes correctes qui lui sont
//    associées.
//
//	  FJ:  Copie-coller de la fonction MyRegisterClass générée automatiquement
//
ATOM WindowsEngine::MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex{};

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = &WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	//wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PETITMOTEUR3D));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	//wcex.lpszMenuName = MAKEINTRESOURCE(IDC_PETITMOTEUR3D);
	wcex.lpszClassName = szWindowClass;
	//wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

int WindowsEngine::Show()
{
	ShowWindow(hMainWnd, SW_SHOWNORMAL);
	UpdateWindow(hMainWnd);

	return 0;
}

//
//	FONCTION : RunSpecific
//
//	BUT :	Éléments internes de la boucle de l'application (Boucle message)
//
//	NOTES:
//		Il s'agit d'une version modifiée spécifiquement pour nos besoins des
//		éléments de la boucle message de Windows
//
bool WindowsEngine::runPlatform()
{
	MSG msg;
	bool bBoucle = true;

	// Y-a-t'il un message Windows à traiter?
	if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		// Est-ce un message de fermeture ?
		if (msg.message == WM_QUIT)
		{
			bBoucle = false;
		}

		// distribuer le message
		if (!::TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
	
	
	
	
	return bBoucle;
}

int WindowsEngine::initPlatform()
{
	// Initialisations de l'application;
	InitAppInstance();
	Show();

	return 0;
}

//  FONCTION : WndProc(HWND, unsigned, WORD, LONG)
//
//  BUT :  traite les messages pour la fenêtre principale.
//
//  WM_COMMAND	- traite le menu de l'application
//  WM_PAINT	- dessine la fenêtre principale
//  WM_DESTROY	- génère un message d'arrêt et retourne
//
//
LRESULT CALLBACK WindowsEngine::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	
	/**********************************************************************/
	case WM_KEYUP:
		wKbd->onKeyReleased(static_cast<unsigned char>(wParam));
		break;
	case WM_CHAR:
		wKbd->onChar(static_cast<char>(wParam));
		break;
	case WM_KEYDOWN:
		if (!(lParam & 0x40000000) || wKbd->autorepeatIsEnable()) {
			std::cout << wParam << std::endl;
 			wKbd->onKeyPressed(static_cast<unsigned char>(wParam));
		}
		break;
	case WM_KILLFOCUS:
		wKbd->clearStates();
		break;
	/**********************************************************************/

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// Évitez d'ajouter du code ici...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Gestionnaire de messages pour la boîte de dialogue À propos de.
INT_PTR CALLBACK WindowsEngine::About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

} // namespace PM3D
