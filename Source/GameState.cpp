/**************************************************************************************\
** File: GameState.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the GameState class
**
\**************************************************************************************/
#include "pch.h"
#include "GameState.h"
#include "AppState.h"
#include "Game.h"
#include "ShopSettings.h"
#include "GUISettings.h"
#include "GUIStrings.h"
namespace Space
{
	void GameState::Init()
	{
		m_menu.SetViewRect();
		m_menu.SetTitleColor(GUISettings::Menu::Text::Color::TITLE);
		m_menu.SetSubtitleColor(GUISettings::Menu::Text::Color::SUBTITLE);
		m_menu.SetTitleFont(&m_titleFont);
		m_menu.SetSubtitleFont(&m_subtitleFont);
		m_menu.SetButtonFont(&m_buttonFont);
		m_menu.SetTitleTextSize(GUISettings::Menu::Text::Size::TITLE);
		m_menu.SetSubtitleTextSize(GUISettings::Menu::Text::Size::SUBTITLE);
		m_menu.SetButtonTextSize(GUISettings::Menu::Text::Size::BUTTON);

		m_showFPS = false;
		ResetController();

		m_shop.Init(ShopSettings::ROOM_LIST);
		m_game.NewGame();
		StartActionMode(true);
	}
	AppStateID GameState::Update(float dt)
	{
		if(m_mode == GameMode::ACTION)
		{
			UpdatePlayerController();
			if(m_game.DidPlayerExit())
				StartPostLevel();
		}
		else
		{
			// Get menu button pressed
			d2d::MenuButton pressedButton;
			bool pressed = m_menu.PollPressedButton(pressedButton);
			if(pressed)
			{
				if(m_mode == GameMode::PAUSED)
				{
					// Pause menu
					if(pressedButton.label == GUIStrings::PauseMenu::QUIT)
						return AppStateID::MAIN_MENU;
					else if(pressedButton.label == GUIStrings::PauseMenu::RESUME)
						UnpauseGame();
				}
				else if(m_mode == GameMode::POST_LEVEL)
				{
					// Post-level main menu
					if(pressedButton.label == GUIStrings::PostLevelMenu::QUIT)
						return AppStateID::MAIN_MENU;
					if(pressedButton.label == GUIStrings::PostLevelMenu::NEXT_LEVEL)
						StartActionMode(true);
					else if(pressedButton.label == GUIStrings::PostLevelMenu::SHOP)
						StartShopMain();
				}
				else if(m_mode == GameMode::SHOP_MAIN)
				{
					// Shop main menu
					if(pressedButton.label == GUIStrings::ShopMenu::BACK)
						StartPostLevel();
					else
						StartShopRoom(pressedButton.label);
				}
				else if(m_mode == GameMode::SHOP_ROOM)
				{
					// Shop room submenus
					std::string roomName = m_menu.GetTitle();
					if(pressedButton.label == GUIStrings::ShopMenu::BACK_ROOM)
						StartShopMain(roomName);
					else
					{
						// Purchase upgrade
						ShopItemID itemID = GetShopMenuButtonID(pressedButton);
						float price = GetShopMenuButtonPrice(pressedButton);
						if(m_game.PurchaseUpgrade(itemID, price))
						{
							m_shop.RemoveItems({ itemID });
							unsigned buttonIndex = m_menu.GetSelectedButtonIndex();
							StartShopRoom(roomName);
							m_menu.SetSelectedButton(buttonIndex);
						}
					}
				}
			}
		}

		// Game
		if(m_mode != GameMode::PAUSED)
			m_game.Update(dt, m_playerController);

		return AppStateID::GAME;
	}
	void GameState::StartActionMode(bool startLevel)
	{
		m_mode = GameMode::ACTION;
		if(startLevel)
			m_game.StartCurrentLevel();
	}
	void GameState::StartPauseMenu()
	{
		m_mode = GameMode::PAUSED;
		m_menu.ClearButtons();

		m_menu.SetTitle(GUIStrings::PauseMenu::TITLE);
		m_menu.SetSubtitle();
		m_menu.SetBackgroundColor(GUISettings::Menu::BackgroundColor::PAUSE);

		d2d::MenuButton button;
		button.label = GUIStrings::PauseMenu::QUIT;
		button.style = GUISettings::Menu::ButtonStyles::Normal::BACK;
		button.highlightStyle = GUISettings::Menu::ButtonStyles::Highlight::BACK;
		m_menu.AddButton(button);
		button.label = GUIStrings::PauseMenu::RESUME;
		button.style = GUISettings::Menu::ButtonStyles::Normal::PRIMARY;
		button.highlightStyle = GUISettings::Menu::ButtonStyles::Highlight::PRIMARY;
		m_menu.AddButton(button, true);
	}
	void GameState::StartPostLevel()
	{
		m_mode = GameMode::POST_LEVEL;
		m_menu.SetTitle(GUIStrings::PostLevelMenu::TITLE);
		m_menu.SetSubtitle();
		m_menu.SetBackgroundColor(GUISettings::Menu::BackgroundColor::POST_LEVEL);
		m_menu.ClearButtons();

		d2d::MenuButton button;
		button.label = GUIStrings::PostLevelMenu::QUIT;
		button.style = GUISettings::Menu::ButtonStyles::Normal::BACK;
		button.highlightStyle = GUISettings::Menu::ButtonStyles::Highlight::BACK;
		m_menu.AddButton(button);

		button.label = GUIStrings::PostLevelMenu::SHOP;
		if(CanPlayerAffordAnyItems())
		{
			button.style = GUISettings::Menu::ButtonStyles::Normal::SECONDARY;
			button.highlightStyle = GUISettings::Menu::ButtonStyles::Highlight::SECONDARY;
		}
		else
		{
			button.style = GUISettings::Menu::ButtonStyles::Normal::GRAYED;
			button.highlightStyle = GUISettings::Menu::ButtonStyles::Highlight::GRAYED;
		}
		m_menu.AddButton(button);

		button.label = GUIStrings::PostLevelMenu::NEXT_LEVEL;
		button.style = GUISettings::Menu::ButtonStyles::Normal::PRIMARY;
		button.highlightStyle = GUISettings::Menu::ButtonStyles::Highlight::PRIMARY;
		m_menu.AddButton(button, true);
	}
	void GameState::StartShopMain(std::string selectedButtonName)
	{
		m_mode = GameMode::SHOP_MAIN;
		m_menu.SetTitle(ShopSettings::TITLE);
		m_menu.SetSubtitle(GetCreditsString());
		m_menu.SetBackgroundColor(GUISettings::Menu::BackgroundColor::POST_LEVEL);
		m_menu.ClearButtons();

		d2d::MenuButton button;
		button.label = GUIStrings::ShopMenu::BACK;
		button.style = GUISettings::Menu::ButtonStyles::Normal::BACK;
		button.highlightStyle = GUISettings::Menu::ButtonStyles::Highlight::BACK;
		m_menu.AddButton(button, true);

		auto roomNames = m_shop.GetRoomNames();
		for(const auto& roomName : roomNames)
		{
			if(CanPlayerAffordAnyItems(roomName))
			{
				button.style = GUISettings::Menu::ButtonStyles::Normal::SECONDARY;
				button.highlightStyle = GUISettings::Menu::ButtonStyles::Highlight::SECONDARY;
			}
			else
			{
				button.style = GUISettings::Menu::ButtonStyles::Normal::GRAYED;
				button.highlightStyle = GUISettings::Menu::ButtonStyles::Highlight::GRAYED;
			}
			button.label = roomName;
			bool select = (roomName == selectedButtonName);
			m_menu.AddButton(button, select);
		}
	}
	void GameState::StartShopRoom(std::string roomName)
	{
		m_mode = GameMode::SHOP_ROOM;
		m_menu.SetTitle(roomName);
		m_menu.SetSubtitle(GetCreditsString());
		m_menu.SetBackgroundColor(GUISettings::Menu::BackgroundColor::POST_LEVEL);
		m_menu.ClearButtons();

		d2d::MenuButton button;
		button.label = GUIStrings::ShopMenu::BACK_ROOM;
		button.style = GUISettings::Menu::ButtonStyles::Normal::BACK;
		button.highlightStyle = GUISettings::Menu::ButtonStyles::Highlight::BACK;
		m_menu.AddButton(button, true);

		const auto& items = m_shop.GetItems(roomName);
		for(const auto& item : items)
		{
			if(m_game.GetPlayerCredits() >= item.price)
			{
				button.style = GUISettings::Menu::ButtonStyles::Normal::SECONDARY;
				button.highlightStyle = GUISettings::Menu::ButtonStyles::Highlight::SECONDARY;
			}
			else
			{
				button.style = GUISettings::Menu::ButtonStyles::Normal::GRAYED;
				button.highlightStyle = GUISettings::Menu::ButtonStyles::Highlight::GRAYED;
			}
			button.label = item.name + "    Cost: "s + d2d::ToString(item.price);
			SetShopMenuButtonID(button, item.id);
			SetShopMenuButtonPrice(button, item.price);
			m_menu.AddButton(button);
		}
	}
	bool GameState::CanPlayerAffordAnyItems(const std::string& roomName) const
	{
		const auto& items = m_shop.GetItems(roomName);
		for(const auto& item : items)
		{
			if(m_game.GetPlayerCredits() >= item.price)
				return true;
		}
		return false;
	}
	std::string GameState::GetCreditsString() const
	{
		return "Credits: "s + d2d::ToString(m_game.GetPlayerCredits());
	}
	void GameState::Draw()
	{
		d2d::Window::SetShowCursor(m_mode != GameMode::ACTION);
		m_game.Draw();
		if(m_mode != GameMode::ACTION)
		{
			d2d::Window::SetViewRect();
			m_menu.Draw();
		}
		if(m_showFPS)
			DrawFPS();
	}
	void GameState::DrawFPS()
	{
		d2d::Window::SetViewRect();
		b2Vec2 resolution{ d2d::Window::GetViewSize() };
		d2d::Window::SetCameraRect({ b2Vec2_zero, resolution });

		int fpsInt{ (int)(d2d::Window::GetFPS() + 0.5f) };
		std::string fpsString{ d2d::ToString(fpsInt) };

		d2d::Window::DisableTextures();
		d2d::Window::EnableBlending();
		d2d::Window::SetColor(GUISettings::HUD::Text::Color::FPS);
		d2d::Window::PushMatrix();
		d2d::Window::Translate(GUISettings::HUD::Text::Position::FPS * resolution);
		d2d::Window::DrawString(fpsString, GUISettings::HUD::Text::Size::FPS * resolution.y,
			m_HUDFont, GUISettings::HUD::Text::Position::FPS_ALIGNMENT);
		d2d::Window::PopMatrix();
	}

	void GameState::ProcessEvent(const SDL_Event& event)
	{
		if(m_mode != GameMode::ACTION)
			m_menu.ProcessEvent(event);

		switch(event.type)
		{
		case SDL_WINDOWEVENT:
			switch(event.window.event)
			{
			case SDL_WINDOWEVENT_FOCUS_LOST:
				PauseGame();
				break;
			}
			break;
		case SDL_KEYDOWN:	ProcessKeyDown(event.key.keysym.sym);	break;
		case SDL_KEYUP:		ProcessKeyUp(event.key.keysym.sym);		break;
		case SDL_CONTROLLERBUTTONDOWN:	ProcessButtonDown(event.cbutton.button);	break;
		case SDL_CONTROLLERBUTTONUP:	ProcessButtonUp(event.cbutton.button);		break;
		case SDL_CONTROLLERAXISMOTION:	ProcessAxisMotion(event.caxis.axis, event.caxis.value);	break;
		case SDL_CONTROLLERDEVICEREMOVED:	ProcessControllerRemoved();	break;
		default: break;
		}
	}
	void GameState::PauseGame()
	{
		if(m_mode == GameMode::ACTION)
			StartPauseMenu();
	}
	void GameState::UnpauseGame()
	{
		if(m_mode == GameMode::PAUSED)
			StartActionMode();
	}
	void GameState::ProcessButtonDown(Uint8 button)
	{
		if(button == m_gamepad.map.pauseButton)						PauseGame();
		else if(button == m_gamepad.map.zoomInButton)				m_gamepad.zoomIn = true;
		else if(button == m_gamepad.map.zoomOutButton)				m_gamepad.zoomOut = true;
		else if(button == m_gamepad.map.previousMissileTypeButton) {}
		else if(button == m_gamepad.map.nextMissileTypeButton) {}
		else if(button == m_gamepad.map.boostButton)				m_gamepad.boost = true;
	}
	void GameState::ProcessButtonUp(Uint8 button)
	{
		if(button == m_gamepad.map.zoomInButton)		m_gamepad.zoomIn = false;
		else if(button == m_gamepad.map.zoomOutButton)	m_gamepad.zoomOut = false;
		else if(button == m_gamepad.map.boostButton)	m_gamepad.boost = false;
	}
	void GameState::ProcessKeyDown(SDL_Keycode key)
	{
		if(key == m_keyboard.map.pauseKey)					PauseGame();
		else if(key == m_keyboard.map.fpsToggleKey)			m_showFPS = !m_showFPS;
		else if(key == m_keyboard.map.zoomInKey)			m_keyboard.zoomIn = true;
		else if(key == m_keyboard.map.zoomOutKey)			m_keyboard.zoomOut = true;
		else if(key == m_keyboard.map.turnLeftKey)			m_keyboard.turnLeft = true;
		else if(key == m_keyboard.map.turnRightKey)			m_keyboard.turnRight = true;
		else if(key == m_keyboard.map.thrustKey)			m_keyboard.thrust = true;
		else if(key == m_keyboard.map.brakeKey)				m_keyboard.brake = true;
		else if(key == m_keyboard.map.boostKey)				m_keyboard.boost = true;
		else if(key == m_keyboard.map.primaryFireKey)		m_keyboard.primaryFire = true;
		else if(key == m_keyboard.map.secondaryFireKey)		m_keyboard.secondaryFire = true;
		//else if(key == m_keyboard.map.morphKey)			m_playerController.doMorphOnce = true;
		else if(key == m_keyboard.map.previousMissileKey) {}
		else if(key == m_keyboard.map.nextMissileKey) {}
	}
	void GameState::ProcessKeyUp(SDL_Keycode key)
	{
		if(key == m_keyboard.map.zoomInKey)				m_keyboard.zoomIn = false;
		else if(key == m_keyboard.map.zoomOutKey)		m_keyboard.zoomOut = false;
		else if(key == m_keyboard.map.turnLeftKey)		m_keyboard.turnLeft = false;
		else if(key == m_keyboard.map.turnRightKey)		m_keyboard.turnRight = false;
		else if(key == m_keyboard.map.thrustKey)		m_keyboard.thrust = false;
		else if(key == m_keyboard.map.brakeKey)			m_keyboard.brake = false;
		else if(key == m_keyboard.map.boostKey)			m_keyboard.boost = false;
		else if(key == m_keyboard.map.primaryFireKey)	m_keyboard.primaryFire = false;
		else if(key == m_keyboard.map.secondaryFireKey)	m_keyboard.secondaryFire = false;
	}
	void GameState::ProcessAxisMotion(Uint8 axis, Sint16 value)
	{
		// Convert to percentage factor
		float axisFactor;
		if(axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT || axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT)
			axisFactor = d2d::AxisToUnit(value, m_gamepad.triggerDeadZone, m_gamepad.triggerAliveZone);
		else
			axisFactor = d2d::AxisToUnit(value, m_gamepad.stickDeadZone, m_gamepad.stickAliveZone);
		if(axis == m_gamepad.map.turnAxis)
			m_gamepad.turnFactor = -axisFactor;
		else if(axis == m_gamepad.map.thrustAxis)
		{
			// If left stick down, brake
			if(axisFactor > 0.0f)
			{
				m_gamepad.thrustFactor = 0.0f;
				m_gamepad.brakeFactor = axisFactor;
			}
			// If left stick up, thrust
			else if(axisFactor < 0.0f)
			{
				m_gamepad.thrustFactor = -axisFactor;
				m_gamepad.brakeFactor = 0.0f;
			}
			else
			{
				m_gamepad.thrustFactor = 0.0f;
				m_gamepad.brakeFactor = 0.0f;
			}
		}
		else if(axis == m_gamepad.map.primaryFireAxis)
			m_gamepad.primaryFireFactor = axisFactor;
		else if(axis == m_gamepad.map.secondaryFireAxis)
			m_gamepad.secondaryFireFactor = axisFactor;
	}
	void GameState::ProcessControllerRemoved()
	{
		PauseGame();
		ResetController();
	}
	void GameState::ResetController()
	{
		m_gamepad.Reset();
		m_keyboard.Reset();
	}
	void GameState::UpdatePlayerController()
	{
		// Zooming
		bool zoomIn = m_gamepad.zoomIn || m_keyboard.zoomIn;
		bool zoomOut = m_gamepad.zoomOut || m_keyboard.zoomOut;
		if(zoomIn && !zoomOut)
			m_playerController.zoomOutFactor = -1.0f;
		else if(!zoomIn && zoomOut)
			m_playerController.zoomOutFactor = 1.0f;
		else
			m_playerController.zoomOutFactor = 0.0f;

		// Turning
		if(m_keyboard.turnLeft && !m_keyboard.turnRight)
			m_playerController.turnFactor = 1.0f;
		else if(!m_keyboard.turnLeft && m_keyboard.turnRight)
			m_playerController.turnFactor = -1.0f;
		else
			m_playerController.turnFactor = m_gamepad.turnFactor;

		// Thrusting
		m_playerController.thrustFactor = m_keyboard.thrust ? 1.0f : m_gamepad.thrustFactor;

		// Braking
		m_playerController.brakeFactor = m_keyboard.brake ? 1.0f : m_gamepad.brakeFactor;

		// Boosting
		m_playerController.boost = m_keyboard.boost || m_gamepad.boost;

		// Firing
		m_playerController.primaryFireFactor = m_keyboard.primaryFire ? 1.0f : m_gamepad.primaryFireFactor;
		m_playerController.secondaryFireFactor = m_keyboard.secondaryFire ? 1.0f : m_gamepad.secondaryFireFactor;
	}
}