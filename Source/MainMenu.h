/**************************************************************************************\
** File: MainMenu.h
** Project:
** Author: David Leksen
** Date:
**
** Header file for the MainMenu class
**
\**************************************************************************************/
#pragma once
#include "AppState.h"

namespace Space
{
	class MainMenu : public AppState
	{
	public:
		void Init() override;
		void ProcessEvent(const SDL_Event& event) override;
		AppStateID Update(float dt) override;
		void Draw() override;

	private:
		d2d::Window::FontReference m_orbitronLightFont{ "Fonts\\OrbitronLight.otf" };
		
		const std::string m_newGameString{ "NEW GAME" };
		const std::string m_quitString{ "QUIT" };
		const std::vector<std::string> m_buttonTextList{ m_newGameString, m_quitString };
		const d2d::Color m_buttonTextColor{ 0.0f, 0.5f, 0.8f, 1.0f };
		const unsigned m_buttonFontID{ m_orbitronLightFont.GetID() };
		const float m_buttonFontSize{ 0.035f };

		const std::string m_title{ "Space Blaster" };
		const d2d::Color m_titleColor{ 0.0f, 1.0f, 0.0f, 1.0f };
		const unsigned m_titleFontID{ m_orbitronLightFont.GetID() };
		const float m_titleFontSize{ 0.045f };

		const d2d::Color m_buttonColor{ d2d::COLOR_ZERO };
		const d2d::Color m_buttonHighlightColor{ 1.0f, 1.0f, 1.0f, 0.3f };
		const d2d::Color m_buttonBorderColor{ 0.5f, 0.5f, 0.5f, 0.5f };
		const d2d::Color m_backgroundColor{ d2d::COLOR_ZERO };

		d2d::Menu m_menu{ m_buttonTextList, m_buttonTextColor, m_buttonFontID, m_buttonFontSize,
			m_title, m_titleColor, m_titleFontID, m_titleFontSize,
			m_buttonColor, m_buttonHighlightColor, m_buttonBorderColor, m_backgroundColor };
	};
}
