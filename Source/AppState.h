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
namespace Space
{
	enum class AppStateID
	{
		INTRO, MAIN_MENU, GAME, QUIT
	};
	class AppState
	{
	public:
		virtual void Init() = 0;
		virtual void ProcessEvent(const SDL_Event& event) = 0;
		virtual AppStateID Update(float dt) = 0;
		virtual void Draw() = 0;
	};
}
