/**************************************************************************************\
** File: MainMenuState.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the MainMenuState class
**
\**************************************************************************************/
#include "pch.h"
#include "MainMenuState.h"
#include "AppState.h"

namespace Space
{
	MainMenuState::MainMenuState()
	{
		m_menu.SetTitle(m_title);
		m_menu.SetTitleStyle(m_titleTextStyle);
		m_menu.SetBackgroundColor(m_backgroundColor);

		d2d::MenuButton button;
		button.color = m_buttonColor;
		button.highlightColor = m_buttonHighlightColor;
		button.borderColor = m_buttonBorderColor;
		button.textStyle = m_buttonTextStyle;

		button.label = m_newGameString;
		m_menu.AddButton(button);
		button.label = m_quitString;
		m_menu.AddButton(button);
	}
	void MainMenuState::Init()
	{
		m_menu.Init();
	}
	void MainMenuState::ProcessEvent(const SDL_Event& event)
	{
		m_menu.ProcessEvent(event);
	}
	AppStateID MainMenuState::Update(float dt)
	{
		std::string pressedButton;
		if(m_menu.PollPressedButton(pressedButton))
		{
			if(pressedButton == m_newGameString)
				return AppStateID::GAME;
			else if(pressedButton == m_quitString)
				return AppStateID::QUIT;
		}
		return AppStateID::MAIN_MENU;
	}
	void MainMenuState::Draw()
	{
		m_menu.Draw();
	}
}
