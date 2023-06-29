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
#include "GameInput.h"
#include "Shop.h"
namespace Space
{
	enum class GameMode
	{
		ACTION, PAUSED, 
		POST_LEVEL, SHOP_MAIN, SHOP_ROOM
	};

	class GameState : public AppState
	{
	public:
		GameState(ResourceManager* resourcesPtr, Camera* cameraPtr, Starfield* starfieldPtr);
		void Init() override;
		void ProcessEvent(const SDL_Event& event) override;
		AppStateID Update(float dt) override;
		void Draw() override;

	private:
		void StartActionMode(bool startLevel = false);
		void StartPauseMenu();
		void StartPostLevel();
		void StartShopMain(std::string selectedButtonName = {});
		void StartShopRoom(std::string roomName);
		bool CanPlayerAffordAnyItems(const std::string& roomName = {}) const;
		std::string GetCreditsString() const;

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

		// Game
		GameMode m_mode;
		Game m_game;
		Shop m_shop;
		d2d::Menu m_menu;
		bool m_showFPS;

		// Gamepad input configuration
		Gamepad m_gamepad;
		Keyboard m_keyboard;
		PlayerController m_playerController;

		// Fonts
		const unsigned m_titleFont{ m_resourcesPtr->fonts.orbitronLight };
		const unsigned m_subtitleFont{ m_resourcesPtr->fonts.orbitronLight };
		const unsigned m_buttonFont{ m_resourcesPtr->fonts.orbitronLight };
		const unsigned m_guiFont{ m_resourcesPtr->fonts.orbitronLight };
	};
}
