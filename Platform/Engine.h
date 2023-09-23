#pragma once
#include "../Utils/Singleton.h"
#include "../Utils/Timer.h"
#include "../Graphics/Graphics.h"
#include <cstdint>
#include <functional>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

namespace MS
{

constexpr Timer::count_type FPS_COUNT = 60;
constexpr Timer::value_type FRAME_TIME = 1.0 / static_cast<Timer::value_type>(FPS_COUNT);

template<class _RenderingDevice=Graphics>
concept canRender = requires (_RenderingDevice gfx) {

	{gfx.present()} -> std::same_as<void>;

};

template <class _Platform, class _RenderingDevice=Graphics> 
	requires canRender<_RenderingDevice>
class Engine : public Singleton<_Platform>
{
public:
	virtual void run()
	{

		bool isRunning = true;

		while (isRunning)
		{
			// Propre à la plateforme - (Conditions d'arrêt, interface, messages)
			isRunning = runPlatform();

			// appeler la fonction d'animation
			if (isRunning)
			{
				ImGui_ImplDX11_NewFrame();
				ImGui_ImplWin32_NewFrame();
				ImGui::NewFrame();
				ImGui::ShowDemoWindow(); // Show demo window! :)
				isRunning = animation();
				ImGui::Render();
				ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
			}

		}
	}

	virtual int init()
	{
		// Propre à la plateforme
		initPlatform();

		// * Initialisation du dispositif de rendu
		p_renderingDevice = createRenderingDevicePlatform(GRAPHICS_MODE::WINDOWED);

		// * Initialisation de la scène

		// * Initialisation des paramètres de l'animation et 
		//   préparation de la première image
		initAnimation();

		return 0;
	}

	virtual bool animation() { 


		p_renderingDevice->present();
		return true;
	
	}

	virtual int initAnimation()
	{
		m_nextTime = getTimePlatform();
		m_previousTime = m_nextTime;
		// première Image
		renderScene();
		return true;
	}

	virtual _RenderingDevice* createRenderingDevicePlatform(GRAPHICS_MODE cdsMode) = 0;



protected:
	virtual ~Engine() = default;

	// Spécifiques - Doivent être implantés
	virtual bool runPlatform() = 0;
	virtual int initPlatform() = 0;

	virtual bool renderScene()
	{
		beginRenderScenePlatform();
		endRenderScenePlatform();
		return true;
	}

	virtual void beginRenderScenePlatform() = 0;
	virtual void endRenderScenePlatform() = 0;



	// Gets the time using platform specific calls
	virtual Timer::value_type getTimePlatform() const = 0;

protected:
	// Variables pour le temps de l’animation
	Timer::value_type m_nextTime;
	Timer::value_type m_previousTime;

	_RenderingDevice* p_renderingDevice; // This will store a Graphics() on windows
};



} // namespace PM3D
