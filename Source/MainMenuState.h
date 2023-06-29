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
		using AppState::AppState;
		void Init() override;
		void ProcessEvent(const SDL_Event& event) override;
		AppStateID Update(float dt) override;
		void Draw() override;

	private:
		d2d::Menu m_menu;
		const unsigned m_titleFont{ m_resourcesPtr->fonts.orbitronLight };
		const unsigned m_subtitleFont{ m_resourcesPtr->fonts.orbitronLight };
		const unsigned m_buttonFont{ m_resourcesPtr->fonts.orbitronLight };
	};
}
