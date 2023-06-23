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
#include "Starfield.h"
#include "Camera.h"
#include "GUISettings.h"
namespace Space
{
	MainMenuState::MainMenuState(Camera* cameraPtr, Starfield* starfieldPtr)
		: AppState{ cameraPtr, starfieldPtr }
	{
		m_menu.SetTitle(m_title);
		m_menu.SetTitleStyle(GUISettings::menuTitleTextStyle);
		m_menu.SetBackgroundColor(d2d::COLOR_ZERO);

		d2d::MenuButton button;
		button.label = m_quitString;
		button.style = GUISettings::backButtonStyle;
		m_menu.AddButton(button);

		button.label = m_newGameString;
		button.style = GUISettings::normalButtonStyle;
		m_menu.AddButton(button, true);
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
		// Starfield
		m_cameraPtr->SetPosition(m_cameraPtr->GetPosition() - (dt * STARFIELD_DEFAULT_VELOCITY));
		m_starfieldPtr->Update(m_cameraPtr->GetPosition());

		// Menu
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
		// Starfield
		d2d::Window::SetCameraRect(m_cameraPtr->GetRect());
		m_starfieldPtr->Draw();

		m_menu.Draw();
	}
}
