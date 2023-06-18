/**************************************************************************************\
** File: GameInput.h
** Project: 
** Author: David Leksen
** Date: 
**
** Header file for Gamepad and Keyboard
**
\**************************************************************************************/
#pragma once
namespace Space
{
	//+--------------------------\--------------------------------
	//|	        Gamepad          | 
	//\--------------------------/--------------------------------
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

		bool zoomIn{};
		bool zoomOut{};
		float turnFactor{};
		float thrustFactor{};
		float brakeFactor{};
		bool boost{};
		float primaryFireFactor{};
		float secondaryFireFactor{};

		void Reset()
		{
			zoomIn = false;
			zoomOut = false;
			turnFactor = 0.0f;
			thrustFactor = 0.0f;
			brakeFactor = 0.0f;
			boost = false;
			primaryFireFactor = 0.0f;
			secondaryFireFactor = 0.0f;
		}
	};

	//+--------------------------\--------------------------------
	//|	        Keyboard          | 
	//\--------------------------/--------------------------------
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
			const SDL_Keycode previousMissileKey{ SDLK_q };
			const SDL_Keycode nextMissileKey{ SDLK_e };
		} map;

		bool zoomIn{};
		bool zoomOut{};
		bool turnLeft{};
		bool turnRight{};
		bool thrust{};
		bool brake{};
		bool boost{};
		bool primaryFire{};
		bool secondaryFire{};

		void Reset()
		{
			zoomIn = false;
			zoomOut = false;
			turnLeft = false;
			turnRight = false;
			thrust = false;
			brake = false;
			primaryFire = false;
			secondaryFire = false;
		}
	};
}
