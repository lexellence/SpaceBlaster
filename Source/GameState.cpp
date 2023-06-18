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

namespace Space
{
	void GameState::Init()
	{
		m_showFPS = false;
		ResetController();
		m_game.Init();
		StartActionMode(true);
	}
	AppStateID GameState::Update(float dt)
	{
		if(m_mode == GameMode::PAUSED || m_mode == GameMode::SHOP)
		{
			// Get menu button pressed
			std::string pressedButton;
			bool pressed;
			d2Assert(m_menuPtr);
			pressed = m_menuPtr->PollPressedButton(pressedButton);

			// Quit
			if(pressedButton == m_quitString)
				return AppStateID::MAIN_MENU;

			if(m_mode == GameMode::PAUSED)
			{
				// Pause menu
				if(pressedButton == m_resumeString)
					UnpauseGame();
			}
			else if(m_mode == GameMode::SHOP)
			{
				if(m_shopMode == ShopMode::MAIN)
				{
					// Shop main
					if(pressedButton == m_nextLevelString)
						StartActionMode(true);
					else if(pressedButton == m_weaponsString)
						StartShopMenu(ShopMode::WEAPONS);
					else if(pressedButton == m_protectionString)
						StartShopMenu(ShopMode::PROTECTION);
					else if(pressedButton == m_engineString)
						StartShopMenu(ShopMode::ENGINE);
					else if(pressedButton == m_gadgetsString)
						StartShopMenu(ShopMode::GADGETS);
				}
				else if(m_shopMode == ShopMode::WEAPONS)
				{
					// Shop weapons
					if(pressedButton == m_backString)
						StartShopMenu();
				}
				else if(m_shopMode == ShopMode::PROTECTION)
				{
					// Shop protection
					if(pressedButton == m_backString)
						StartShopMenu();
				}
				else if(m_shopMode == ShopMode::ENGINE)
				{
					// Shop engine
					if(pressedButton == m_backString)
						StartShopMenu();
				}
				else if(m_shopMode == ShopMode::GADGETS)
				{
					// Shop gadgets
					if(pressedButton == m_backString)
						StartShopMenu();
				}
			}
		}
		else if(m_mode == GameMode::ACTION)
		{
			UpdatePlayerController();
			if(m_game.DidPlayerExit())
				StartShopMenu();
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
		m_menuPtr = nullptr;
	}
	void GameState::StartPauseMenu()
	{
		m_mode = GameMode::PAUSED;
		m_pauseMenu.Init();
		m_menuPtr = &m_pauseMenu;
	}
	void GameState::StartShopMenu(ShopMode mode)
	{
		m_mode = GameMode::SHOP;
		m_shopMode = mode;

		if(m_shopMode == ShopMode::MAIN)
			m_menuPtr = &m_shopMenu;
		else if(m_shopMode == ShopMode::WEAPONS)
			m_menuPtr = &m_weaponsMenu;
		else if(m_shopMode == ShopMode::PROTECTION)
			m_menuPtr = &m_protectionMenu;
		else if(m_shopMode == ShopMode::ENGINE)
			m_menuPtr = &m_engineMenu;
		else if(m_shopMode == ShopMode::GADGETS)
			m_menuPtr = &m_gadgetsMenu;

		d2Assert(m_menuPtr);
		m_menuPtr->Init();
	}
	void GameState::Draw()
	{
		d2d::Window::SetShowCursor(m_mode != GameMode::ACTION);
		m_game.Draw();
		if(m_mode == GameMode::PAUSED || m_mode == GameMode::SHOP)
		{
			d2Assert(m_menuPtr);
			m_menuPtr->Draw();
		}
		if(m_showFPS)
			DrawFPS();
	}
	void GameState::DrawFPS()
	{
		// Set camera to screen resolution
		b2Vec2 resolution{ d2d::Window::GetScreenResolution() };
		d2d::Window::SetCameraRect({ b2Vec2_zero, resolution });
		{
			// Convert fps to string and draw
			int fpsInt{ (int)(d2d::Window::GetFPS() + 0.5f) };
			std::string fpsString{ d2d::ToString(fpsInt) };

			d2d::Window::DisableTextures();
			d2d::Window::EnableBlending();
			d2d::Window::SetColor(m_fpsTextStyle.color);
			d2d::Window::PushMatrix();
			d2d::Window::Translate(m_fpsPosition * resolution);
			d2d::Window::DrawString(fpsString, m_fpsAlignment, m_fpsTextStyle.size * resolution.y, m_fpsTextStyle.font);
			d2d::Window::PopMatrix();
		}
	}

	void GameState::ProcessEvent(const SDL_Event& event)
	{
		if(m_mode == GameMode::PAUSED || m_mode == GameMode::SHOP)
		{
			d2Assert(m_menuPtr);
			m_menuPtr->ProcessEvent(event);
		}

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
		else if(button == m_gamepad.map.previousMissileTypeButton)	{}
		else if(button == m_gamepad.map.nextMissileTypeButton)		{}
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
		else if(key == m_keyboard.map.previousMissileKey)	{}
		else if(key == m_keyboard.map.nextMissileKey)		{}
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
