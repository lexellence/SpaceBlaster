/**************************************************************************************\
** File: Gameplay.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the Gameplay class
**
\**************************************************************************************/
#include "pch.h"
#include "Gameplay.h"
#include "AppState.h"
#include "Game.h"

namespace Space
{
	void Gameplay::Init()
	{
		m_game.Init();

		m_paused = false;
		m_showFPS = true;

		m_gamepadZoomEngaged = false;
		m_gamepadZoomOutFactor = 0.0f;
		m_gamepadTurnFactor = 0.0f;
		m_gamepadThrustFactor = 0.0f;
		m_gamepadBrakeFactor = 0.0f;
		m_primaryFireFactor = 0.0f;
		m_secondaryFireFactor = 0.0f;

		m_zoomInKeyPressed = false;
		m_zoomOutKeyPressed = false;
		m_turnLeftKeyPressed = false;
		m_turnRightKeyPressed = false;
		m_thrustKeyPressed = false;
		m_brakeKeyPressed = false;
		m_primaryFireKeyPressed = false;
		m_secondaryFireKeyPressed = false;
	}
	void Gameplay::ProcessEvent(const SDL_Event& event)
	{
		if(m_paused)
			m_pauseMenu.ProcessEvent(event);
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
	AppStateID Gameplay::Update(float dt)
	{
		// If paused, handle any menu selections
		if(m_paused)
		{
			std::string pressedButton;
			if(m_pauseMenu.PollPressedButton(pressedButton))
			{
				if(pressedButton == m_resumeString)
					UnpauseGame();
				else if(pressedButton == m_quitToMenuString)
					return AppStateID::MAIN_MENU;
				else if(pressedButton == m_quitString)
					return AppStateID::QUIT;
			}
		}

		// If not paused, update game
		if(!m_paused)
		{
			UpdatePlayerController();
			m_game.Update(dt, m_playerController);
		}
		return AppStateID::GAMEPLAY;
	}
	void Gameplay::PauseGame()
	{
		if(!m_paused)
		{
			m_paused = true;
			m_pauseMenu.Init();
		}
	}
	void Gameplay::UnpauseGame()
	{
		if(m_paused)
			m_paused = false;
	}
	void Gameplay::ProcessButtonDown(Uint8 button)
	{
		if(button == m_pauseButton)					PauseGame();
		else if(button == m_zoomButton)				m_gamepadZoomEngaged = true;
		else if(button == m_primaryFireButton)		m_primaryFireFactor = 1.0f;
		else if(button == m_secondaryFireButton)	m_secondaryFireFactor = 1.0f;
		else if(button == m_morphButton)			m_playerController.morph = true;
		else if(button == m_previousMissileTypeButton)	--m_playerController.missileTypeChange;
		else if(button == m_nextMissileTypeButton)	++m_playerController.missileTypeChange;
	}
	void Gameplay::ProcessButtonUp(Uint8 button)
	{
		if(button == m_zoomButton)		m_gamepadZoomEngaged = false;
		else if(button == m_primaryFireButton)		m_primaryFireFactor = 0.0f;
		else if(button == m_secondaryFireButton)	m_secondaryFireFactor = 0.0f;
	}
	void Gameplay::ProcessKeyDown(SDL_Keycode key)
	{
		if(key == m_pauseKey)				PauseGame();
		else if(key == m_fpsToggleKey)		m_showFPS = !m_showFPS;
		else if(key == m_zoomInKey)			m_zoomInKeyPressed = true;
		else if(key == m_zoomOutKey)		m_zoomOutKeyPressed = true;
		else if(key == m_turnLeftKey)		m_turnLeftKeyPressed = true;
		else if(key == m_turnRightKey)		m_turnRightKeyPressed = true;
		else if(key == m_thrustKey)			m_thrustKeyPressed = true;
		else if(key == m_brakeKey)			m_brakeKeyPressed = true;
		else if(key == m_primaryFireKey)	m_primaryFireKeyPressed = true;
		else if(key == m_secondaryFireKey)	m_secondaryFireKeyPressed = true;
		else if(key == m_morphKey)			m_playerController.morph = true;
		else if(key == m_previousMissileKey) --m_playerController.missileTypeChange;
		else if(key == m_nextMissileKey)	++m_playerController.missileTypeChange;
	}
	void Gameplay::ProcessKeyUp(SDL_Keycode key)
	{
		if(key == m_zoomInKey)				m_zoomInKeyPressed = false;
		else if(key == m_zoomOutKey)		m_zoomOutKeyPressed = false;
		else if(key == m_turnLeftKey)		m_turnLeftKeyPressed = false;
		else if(key == m_turnRightKey)		m_turnRightKeyPressed = false;
		else if(key == m_thrustKey)			m_thrustKeyPressed = false;
		else if(key == m_brakeKey)			m_brakeKeyPressed = false;
		else if(key == m_primaryFireKey)	m_primaryFireKeyPressed = false;
		else if(key == m_secondaryFireKey)	m_secondaryFireKeyPressed = false;
	}
	void Gameplay::ProcessAxisMotion(Uint8 axis, Sint16 value)
	{
		float axisFactor;
		if(axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT || axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT) 
			axisFactor = d2d::AxisToUnit(value, m_gamepadTriggerDeadZone, m_gamepadTriggerAliveZone);
		else 
			axisFactor = d2d::AxisToUnit(value, m_gamepadStickDeadZone, m_gamepadStickAliveZone);

		if(axis == m_zoomAxis)
			m_gamepadZoomOutFactor = axisFactor;
		else if(axis == m_turnAxis)
			m_gamepadTurnFactor = -axisFactor;
		else if(axis == m_thrustAxis)
		{
			// If left stick down, brake
			if(axisFactor > 0.0f)
			{
				m_gamepadThrustFactor = 0.0f;
				m_gamepadBrakeFactor = axisFactor;
			}
			// If left stick up, thrust
			else if(axisFactor < 0.0f)
			{
				m_gamepadThrustFactor = -axisFactor;
				m_gamepadBrakeFactor = 0.0f;
			}
			else
			{
				m_gamepadThrustFactor = 0.0f;
				m_gamepadBrakeFactor = 0.0f;
			}
		}
		else if(axis == m_primaryFireButton)
			m_primaryFireFactor = axisFactor;
		else if(axis == m_secondaryFireButton)
			m_secondaryFireFactor = axisFactor;
	}
	void Gameplay::ProcessControllerRemoved()
	{
		m_paused = true;
		ResetController();
	}
	void Gameplay::ResetController()
	{
		m_gamepadZoomEngaged = false;
		m_gamepadZoomOutFactor = 0.0f;
		m_gamepadTurnFactor = 0.0f;
		m_gamepadThrustFactor = 0.0f;
		m_gamepadBrakeFactor = 0.0f;
		m_primaryFireFactor = 0.0f;
		m_secondaryFireFactor = 0.0f;

		m_zoomInKeyPressed = false;
		m_zoomOutKeyPressed = false;
		m_turnLeftKeyPressed = false;
		m_turnRightKeyPressed = false;
		m_thrustKeyPressed = false;
		m_brakeKeyPressed = false;
		m_primaryFireKeyPressed = false;
		m_secondaryFireKeyPressed = false;
	}
	void Gameplay::UpdatePlayerController()
	{
		// Zooming
		if(m_zoomInKeyPressed && !m_zoomOutKeyPressed)
			m_playerController.zoomOutFactor = -1.0f;
		else if(!m_zoomInKeyPressed && m_zoomOutKeyPressed)
			m_playerController.zoomOutFactor = 1.0f;
		else if(m_gamepadZoomEngaged)
			m_playerController.zoomOutFactor = m_gamepadZoomOutFactor;
		else
			m_playerController.zoomOutFactor = 0.0f;

		// Turning
		if(m_turnLeftKeyPressed && !m_turnRightKeyPressed)
			m_playerController.turnFactor = 1.0f;
		else if(!m_turnLeftKeyPressed && m_turnRightKeyPressed)
			m_playerController.turnFactor = -1.0f;
		else
			m_playerController.turnFactor = m_gamepadTurnFactor;

		// Thrusting
		if(m_thrustKeyPressed)
			m_playerController.thrustFactor = 1.0f;
		else
			m_playerController.thrustFactor = m_gamepadThrustFactor;

		// Braking
		if(m_brakeKeyPressed)
			m_playerController.brakeFactor = 1.0f;
		else
			m_playerController.brakeFactor = m_gamepadBrakeFactor;

		// Firing
		if(m_primaryFireKeyPressed)
			m_playerController.primaryFireFactor = 1.0f;
		else
			m_playerController.primaryFireFactor = m_primaryFireFactor;

		if(m_secondaryFireKeyPressed)
			m_playerController.secondaryFireFactor = 1.0f;
		else
			m_playerController.secondaryFireFactor = m_secondaryFireFactor;
	}
	void Gameplay::Draw()
	{
		//d2d::Window::SetShowCursor(m_paused);
		m_game.Draw();
		if(m_paused)
			m_pauseMenu.Draw();
		if(m_showFPS)
			DrawFPS();
	}
	void Gameplay::DrawFPS()
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
}
