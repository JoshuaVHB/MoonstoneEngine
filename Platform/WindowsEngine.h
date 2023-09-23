#pragma once

#include "Engine.h"
#include "../Graphics/Graphics.h"

#define MAX_LOADSTRING 100

namespace MS
{

class WindowsEngine final : public Engine<WindowsEngine, Graphics>
{
public:
	void SetWindowsAppInstance(HINSTANCE hInstance);

	Graphics& getGraphics() const { return *m_graphics; }

protected:
	virtual Timer::value_type getTimePlatform() const override;
	virtual Graphics* createRenderingDevicePlatform(GRAPHICS_MODE cdsMode) override;

	Timer::value_type getTimeIntervalInSeconds(
		Timer::count_type  start,
		Timer::count_type stop)	const;


private:

	virtual int initPlatform() override;
	virtual bool runPlatform() override;

protected:
	virtual void beginRenderScenePlatform() override {
	
		ID3D11DeviceContext* pImmediateContext = p_renderingDevice->getImmediateContext();
		ID3D11RenderTargetView* pRenderTargetView = p_renderingDevice->getRenderTargetView();
		// On efface la surface de rendu
		float Couleur[4] = { 0.0f, 0.5f, 0.0f, 1.0f }; // RGBA - Vert pour le moment
		pImmediateContext->ClearRenderTargetView(pRenderTargetView, Couleur);

	
	}
	virtual void endRenderScenePlatform() override {}

private:

	std::unique_ptr<Graphics> m_graphics;
	Timer m_clock;

// =========================== WINDOWS STUFF =========================== //
public:
	HWND getHwnd() const { return hMainWnd; }
private:
	bool InitAppInstance();
	ATOM MyRegisterClass(HINSTANCE hInstance);
	int Show();


	// Fonctions "Callback" -- Doivent être statiques
	static LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
	static INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

	HACCEL hAccelTable;						// handle Windows de la table des accélérateurs
	static HINSTANCE hAppInstance;			// handle Windows de l'instance actuelle de l'application
	HWND hMainWnd;							// handle Windows de la fenêtre principale
	TCHAR szWindowClass[MAX_LOADSTRING];	// le nom de la classe de fenêtre principale
};

} // namespace MS
