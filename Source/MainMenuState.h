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
#include "GUISettings.h"
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
		d2d::FontReference m_titleFont{ GUISettings::Menu::Text::Font::TITLE };
		d2d::FontReference m_subtitleFont{ GUISettings::Menu::Text::Font::SUBTITLE };
		d2d::FontReference m_buttonFont{ GUISettings::Menu::Text::Font::BUTTON };
	};
}
