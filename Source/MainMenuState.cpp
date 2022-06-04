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
		//d2d::Window::SetShowCursor(true);
		m_menu.Draw();
	}
}
