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
#include "GUISettings.h"
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
		using AppState::AppState;
		void Init() override;
		void ProcessEvent(const SDL_Event& event) override;
		AppStateID Update(float dt) override;
		void Draw() override;

	private:
		void MenuGoBack();
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
		Game m_game{ m_cameraPtr, m_starfieldPtr };
		Shop m_shop;
		d2d::Menu m_menu;
		bool m_showFPS;

		// Gamepad input configuration
		Gamepad m_gamepad;
		Keyboard m_keyboard;
		PlayerController m_playerController;

		d2d::FontReference m_titleFont{ GUISettings::Menu::Text::Font::TITLE };
		d2d::FontReference m_subtitleFont{ GUISettings::Menu::Text::Font::SUBTITLE };
		d2d::FontReference m_buttonFont{ GUISettings::Menu::Text::Font::BUTTON };
		d2d::FontReference m_HUDFont{ GUISettings::HUD::Text::Font::DEFAULT };
	};
}
