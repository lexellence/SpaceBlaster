/**************************************************************************************\
** File: MainMenu.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the MainMenu class
**
\**************************************************************************************/
#include "pch.h"
#include "MainMenu.h"
#include "AppState.h"

namespace Space
{
	void MainMenu::Init()
	{
		m_menu.Init();
	}
	void MainMenu::ProcessEvent(const SDL_Event& event)
	{
		m_menu.ProcessEvent(event);
	}
	AppStateID MainMenu::Update(float dt)
	{
		std::string pressedButton;
		if(m_menu.PollPressedButton(pressedButton))
		{
			if(pressedButton == m_newGameString)
				return AppStateID::GAMEPLAY;
			else if(pressedButton == m_quitString)
				return AppStateID::QUIT;
		}
		return AppStateID::MAIN_MENU;
	}
	void MainMenu::Draw()
	{
		//d2d::Window::SetShowCursor(true);
		m_menu.Draw();
	}
}
