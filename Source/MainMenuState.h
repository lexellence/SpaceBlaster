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
		MainMenuState();
		void Init() override;
		void ProcessEvent(const SDL_Event& event) override;
		AppStateID Update(float dt) override;
		void Draw() override;

	private:
		d2d::FontReference m_orbitronLightFont{ "Fonts/OrbitronLight.otf" };

		const std::string m_newGameString{ "New Game" };
		const std::string m_quitString{ "Quit" };
		const std::vector<std::string> m_buttonTextList{ m_newGameString, m_quitString };
		const d2d::TextStyle m_buttonTextStyle{ &m_orbitronLightFont,
			{ 0.0f, 0.5f, 0.8f, 1.0f }, 0.035f };

		const std::string m_title{ "Space Blaster" };
		const d2d::TextStyle m_titleTextStyle{ &m_orbitronLightFont,
			{ 0.0f, 1.0f, 0.0f, 1.0f }, 0.045f };

		const d2d::Color m_buttonColor{ d2d::COLOR_ZERO };
		const d2d::Color m_buttonHighlightColor{ 1.0f, 1.0f, 1.0f, 0.3f };
		const d2d::Color m_buttonBorderColor{ 0.5f, 0.5f, 0.5f, 0.5f };
		const d2d::Color m_backgroundColor{ d2d::COLOR_ZERO };

		d2d::Menu m_menu;
	};
}
