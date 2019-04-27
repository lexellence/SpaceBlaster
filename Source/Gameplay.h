/**************************************************************************************\
** File: Gameplay.h
** Project:
** Author: David Leksen
** Date:
**
** Header file for the Gameplay class
**
\**************************************************************************************/
#pragma once
#include "AppState.h"
#include "Game.h"

namespace Space
{
	class Gameplay : public AppState
	{
	public:
		void Init() override;
		void ProcessEvent(const SDL_Event& event) override;
		AppStateID Update(float dt) override;
		void Draw() override;

	private:
		void PauseGame();
		void UnpauseGame();
		void ProcessKeyDown(SDL_Keycode key);
		void ProcessKeyUp(SDL_Keycode key);
		void ProcessButtonDown(Uint8 button);
		void ProcessButtonUp(Uint8 button);
		void ProcessAxisMotion(Uint8 axis, Sint16 value);
		void ProcessControllerRemoved();
		void ResetController();

		void UpdatePlayerController();
		void DrawFPS();

		Game m_game;

		// Gamepad input configuration
		PlayerController m_playerController;
		const float m_gamepadTriggerDeadZone{ 1000.0f };
		const float m_gamepadTriggerAliveZone{ 2000.0f };
		const float m_gamepadStickDeadZone{ 6700.0f };
		const float m_gamepadStickAliveZone{ 2000.0f };
		const Uint8 m_pauseButton{ SDL_CONTROLLER_BUTTON_START };
		const Uint8 m_zoomButton{ SDL_CONTROLLER_BUTTON_LEFTSHOULDER };
		const Uint8 m_zoomAxis{ SDL_CONTROLLER_AXIS_RIGHTY };
		bool m_gamepadZoomEngaged;
		float m_gamepadZoomOutFactor;
		const Uint8 m_turnAxis{ SDL_CONTROLLER_AXIS_RIGHTX };
		float m_gamepadTurnFactor;
		const Uint8 m_thrustAxis{ SDL_CONTROLLER_AXIS_LEFTY };
		float m_gamepadThrustFactor;
		float m_gamepadBrakeFactor;
		const Uint8 m_primaryFireButton{ SDL_CONTROLLER_AXIS_TRIGGERRIGHT };
		const Uint8 m_secondaryFireButton{ SDL_CONTROLLER_AXIS_TRIGGERLEFT };
		float m_primaryFireFactor;
		float m_secondaryFireFactor;
		const Uint8 m_morphButton{ SDL_CONTROLLER_BUTTON_Y };
		const Uint8 m_previousMissileTypeButton{ SDL_CONTROLLER_BUTTON_DPAD_LEFT };
		const Uint8 m_nextMissileTypeButton{ SDL_CONTROLLER_BUTTON_DPAD_RIGHT };

		// Keyboard input configuration
		const SDL_Keycode m_pauseKey{ SDLK_ESCAPE };
		bool m_paused;
		const SDL_Keycode m_fpsToggleKey{ SDLK_F12 };
		bool m_showFPS;
		const SDL_Keycode m_zoomInKey{ SDLK_PAGEUP };
		const SDL_Keycode m_zoomOutKey{ SDLK_PAGEDOWN };
		bool m_zoomInKeyPressed;
		bool m_zoomOutKeyPressed;
		const SDL_Keycode m_turnLeftKey{ SDLK_LEFT };
		const SDL_Keycode m_turnRightKey{ SDLK_RIGHT };
		bool m_turnLeftKeyPressed;
		bool m_turnRightKeyPressed;
		const SDL_Keycode m_thrustKey{ SDLK_UP };
		const SDL_Keycode m_brakeKey{ SDLK_DOWN };
		bool m_thrustKeyPressed;
		bool m_brakeKeyPressed;
		const SDL_Keycode m_primaryFireKey{ SDLK_SPACE };
		const SDL_Keycode m_secondaryFireKey{ SDLK_LSHIFT };
		bool m_primaryFireKeyPressed;
		bool m_secondaryFireKeyPressed;
		const SDL_Keycode m_morphKey{ SDLK_TAB };
		const SDL_Keycode m_previousMissileKey{ SDLK_q };
		const SDL_Keycode m_nextMissileKey{ SDLK_e };

		d2d::Window::FontReference m_orbitronLightFont{ std::string{"Fonts\\OrbitronLight.otf"} };

		// Pause menu button text
		std::string m_resumeString{ "RESUME" };
		std::string m_quitToMenuString{ "QUIT TO MAIN MENU" };
		std::string m_quitString{ "QUIT TO OPERATING SYSTEM" };
		const std::vector<std::string> m_buttonTextList{ m_resumeString, m_quitToMenuString, m_quitString };
		const d2d::Color m_buttonTextColor{ 0.0f, 0.5f, 0.8f, 1.0f };
		const unsigned m_buttonFontID{ m_orbitronLightFont.GetID() };
		const float m_buttonFontSize{ 0.035f };

		// Pause menu title text
		const std::string m_title{ "Paused" };
		const d2d::Color m_titleColor{ 0.8f, 0.8f, 0.8f, 0.8f };
		const unsigned m_titleFontID{ m_orbitronLightFont.GetID() };
		const float m_titleFontSize{ 0.045f };

		// Pause menu button colors
		const d2d::Color m_buttonColor{ d2d::COLOR_ZERO };
		const d2d::Color m_buttonHighlightColor{ 1.0f, 1.0f, 1.0f, 0.3f };
		const d2d::Color m_buttonBorderColor{ 0.5f, 0.5f, 0.5f, 0.5f };
		const d2d::Color m_backgroundColor{ 0.2f, 0.2f, 0.2f, 0.4f };

		d2d::Menu m_pauseMenu{ m_buttonTextList, m_buttonTextColor, m_buttonFontID, m_buttonFontSize,
			m_title, m_titleColor, m_titleFontID, m_titleFontSize,
			m_buttonColor, m_buttonHighlightColor, m_buttonBorderColor, m_backgroundColor };

		// FPS display
		const b2Vec2 m_fpsPosition{ 1.0f, 1.0f };
		const d2d::Alignment m_fpsAlignment{ d2d::Alignment::RIGHT_TOP };
		const float m_fpsFontSize{ 0.05f };
		const unsigned m_fpsFontID{ m_orbitronLightFont.GetID() };
		const d2d::Color m_fpsColor{ 1.0f, 1.0f, 0.0f, 1.0f };
	};
}
