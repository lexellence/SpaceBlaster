/**************************************************************************************\
** File: AppState.h
** Project:
** Author: David Leksen
** Date:
**
** Header file for the AppState interface enum
**
\**************************************************************************************/
#pragma once
union SDL_Event;
#include "Camera.h"
#include "Starfield.h"
#include "Resources.h"
namespace Space
{
	enum class AppStateID
	{
		INTRO, MAIN_MENU, GAME, QUIT
	};
	class AppState
	{
	public:
		AppState() = delete;
		AppState(ResourceManager* resourcesPtr, Camera* cameraPtr, Starfield* starfieldPtr)
			: m_resourcesPtr{ resourcesPtr},
			m_cameraPtr{ cameraPtr }, 
			m_starfieldPtr{ starfieldPtr }
		{
			d2Assert(m_resourcesPtr);
			d2Assert(m_cameraPtr);
			d2Assert(m_starfieldPtr);
		}
		virtual void Init() = 0;
		virtual void ProcessEvent(const SDL_Event& event) = 0;
		virtual AppStateID Update(float dt) = 0;
		virtual void Draw() = 0;
	protected:
		ResourceManager *const m_resourcesPtr;
		Camera *const m_cameraPtr;
		Starfield *const m_starfieldPtr;
	};
}
