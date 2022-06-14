/**************************************************************************************\
** File: GameState.h
** Project:
** Author: David Leksen
** Date:
**
** Header file for the GameState class
**
\**************************************************************************************/
#pragma once
#include "AppState.h"
#include "Game.h"

namespace Space
{
	class GameState : public AppState
	{
	public:
		void Init() override;
		void ProcessEvent(const SDL_Event& event) override;
		AppStateID Update(float dt) override;
		void Draw() override;

	private:
		void PauseGame();
		void UnpauseGame();
		void LessMissiles();
		void MoreMissiles();
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
		bool m_paused;
		bool m_showFPS;

		// Gamepad input configuration
		PlayerController m_playerController;
		struct Gamepad
		{
			// Deadzone
			const float triggerDeadZone{ 1000.0f };
			const float triggerAliveZone{ 2000.0f };
			const float stickDeadZone{ 6700.0f };
			const float stickAliveZone{ 2000.0f };

			// Button/Axis
			struct ControllerMapping
			{
				const Uint8 pauseButton{ SDL_CONTROLLER_BUTTON_START };
				const Uint8 zoomInButton{ SDL_CONTROLLER_BUTTON_DPAD_UP };
				const Uint8 zoomOutButton{ SDL_CONTROLLER_BUTTON_DPAD_DOWN };
				const Uint8 turnAxis{ SDL_CONTROLLER_AXIS_RIGHTX };
				const Uint8 thrustAxis{ SDL_CONTROLLER_AXIS_LEFTY };
				const Uint8 boostButton{ SDL_CONTROLLER_BUTTON_LEFTSHOULDER };
				const Uint8 primaryFireAxis{ SDL_CONTROLLER_AXIS_TRIGGERRIGHT };
				const Uint8 secondaryFireAxis{ SDL_CONTROLLER_AXIS_TRIGGERLEFT };
				//const Uint8 m_morphButton{ SDL_CONTROLLER_BUTTON_Y };
				const Uint8 previousMissileTypeButton{ SDL_CONTROLLER_BUTTON_DPAD_LEFT };
				const Uint8 nextMissileTypeButton{ SDL_CONTROLLER_BUTTON_DPAD_RIGHT };
			} map;

			bool zoomIn;
			bool zoomOut;
			float turnFactor;
			float thrustFactor;
			float brakeFactor;
			bool boost;
			float primaryFireFactor;
			float secondaryFireFactor;
		} m_gamepad;
		struct Keyboard
		{
			struct KeyboardMapping
			{
				const SDL_Keycode pauseKey{ SDLK_ESCAPE };
				const SDL_Keycode fpsToggleKey{ SDLK_F12 };
				const SDL_Keycode zoomInKey{ SDLK_PAGEUP };
				const SDL_Keycode zoomOutKey{ SDLK_PAGEDOWN };
				const SDL_Keycode turnLeftKey{ SDLK_LEFT };
				const SDL_Keycode turnRightKey{ SDLK_RIGHT };
				const SDL_Keycode thrustKey{ SDLK_UP };
				const SDL_Keycode brakeKey{ SDLK_DOWN };
				const SDL_Keycode boostKey{ SDLK_LCTRL };
				const SDL_Keycode primaryFireKey{ SDLK_SPACE };
				const SDL_Keycode secondaryFireKey{ SDLK_LSHIFT };
				//const SDL_Keycode morphKey{ SDLK_TAB };
				const SDL_Keycode previousMissileKey{ SDLK_q };
				const SDL_Keycode nextMissileKey{ SDLK_e };
			} map;

			bool zoomIn;
			bool zoomOut;
			bool turnLeft;
			bool turnRight;
			bool thrust;
			bool brake;
			bool boost;
			bool primaryFire;
			bool secondaryFire;
		} m_keyboard;

		d2d::FontReference m_orbitronLightFont{ "Fonts\\OrbitronLight.otf"s };

		// Pause menu button text
		std::string m_resumeString{ "RESUME" };
		std::string m_quitToMenuString{ "QUIT TO MAIN MENU" };
		std::string m_quitString{ "QUIT TO OPERATING SYSTEM" };
		const std::vector<std::string> m_buttonTextList{ m_resumeString, m_quitToMenuString, m_quitString };
		const d2d::TextStyle m_buttonTextStyle{
			m_orbitronLightFont,
			{ 0.0f, 0.5f, 0.8f, 1.0f },
			0.035f
		};

		// Pause menu title text
		const std::string m_title{ "Paused" };
		const d2d::TextStyle m_titleTextStyle{
			m_orbitronLightFont,
			{ 0.8f, 0.8f, 0.8f, 0.8f },
			0.045f
		};

		// Pause menu button colors
		const d2d::Color m_buttonColor{ d2d::COLOR_ZERO };
		const d2d::Color m_buttonHighlightColor{ 1.0f, 1.0f, 1.0f, 0.3f };
		const d2d::Color m_buttonBorderColor{ 0.5f, 0.5f, 0.5f, 0.5f };
		const d2d::Color m_backgroundColor{ 0.2f, 0.2f, 0.2f, 0.4f };

		d2d::Menu m_pauseMenu{ m_buttonTextList, m_buttonTextStyle,
			m_title, m_titleTextStyle,
			m_buttonColor, m_buttonHighlightColor, 
			m_buttonBorderColor, m_backgroundColor };

		// FPS display
		const b2Vec2 m_fpsPosition{ 1.0f, 1.0f };
		const d2d::Alignment m_fpsAlignment{ d2d::Alignment::RIGHT_TOP };
		const d2d::TextStyle m_fpsTextStyle{
			m_orbitronLightFont,
			{ 1.0f, 1.0f, 0.0f, 1.0f },
			0.05f
		};
	};
}
