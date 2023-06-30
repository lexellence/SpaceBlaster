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
#include "GUIStrings.h"
namespace Space
{
	void MainMenuState::Init()
	{
		m_menu.SetViewRect();
		m_menu.SetBackgroundColor(GUISettings::Menu::BackgroundColor::MAIN);
		m_menu.SetTitleColor(GUISettings::Menu::Text::Color::TITLE);
		m_menu.SetSubtitleColor(GUISettings::Menu::Text::Color::SUBTITLE);
		m_menu.SetTitleFont(&m_titleFont);
		m_menu.SetSubtitleFont(&m_subtitleFont);
		m_menu.SetButtonFont(&m_buttonFont);
		m_menu.SetTitleTextSize(GUISettings::Menu::Text::Size::TITLE);
		m_menu.SetSubtitleTextSize(GUISettings::Menu::Text::Size::SUBTITLE);
		m_menu.SetButtonTextSize(GUISettings::Menu::Text::Size::BUTTON);

		m_menu.SetTitle(GUIStrings::MainMenu::TITLE);
		m_menu.SetSubtitle();

		m_menu.ClearButtons();
		d2d::MenuButton button;
		button.label = GUIStrings::MainMenu::QUIT;
		button.style = GUISettings::Menu::ButtonStyles::Normal::BACK;
		button.highlightStyle = GUISettings::Menu::ButtonStyles::Highlight::BACK;
		m_menu.AddButton(button);

		button.label = GUIStrings::MainMenu::NEW_GAME;
		button.style = GUISettings::Menu::ButtonStyles::Normal::PRIMARY;
		button.highlightStyle = GUISettings::Menu::ButtonStyles::Highlight::PRIMARY;
		m_menu.AddButton(button, true);

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
			if(pressedButton == GUIStrings::MainMenu::NEW_GAME)
				return AppStateID::GAME;
			else if(pressedButton == GUIStrings::MainMenu::QUIT)
				return AppStateID::QUIT;
		}
		return AppStateID::MAIN_MENU;
	}
	void MainMenuState::Draw()
	{
		// Starfield
		d2d::Window::SetViewRect();
		d2d::Window::SetCameraRect(m_cameraPtr->GetRect());
		m_starfieldPtr->Draw();

		m_menu.Draw();
	}
}