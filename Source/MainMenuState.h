/**************************************************************************************\
** File: MainMenuState.h
** Project:
** Author: David Leksen
** Date:
**
** Header file for the MainMenuState class
**
\**************************************************************************************/
#pragma once
#include "AppState.h"
namespace Space
{
	class MainMenuState : public AppState
	{
	public:
		MainMenuState(Camera* cameraPtr, Starfield* starfieldPtr);
		void Init() override;
		void ProcessEvent(const SDL_Event& event) override;
		AppStateID Update(float dt) override;
		void Draw() override;

	private:
		const std::string m_newGameString{ "New Game" };
		const std::string m_quitString{ "Quit" };
		const std::string m_title{ "Space Blaster" };
		d2d::Menu m_menu;
	};
}
