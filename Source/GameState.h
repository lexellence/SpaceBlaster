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
namespace Space
{
	enum class GameMode
	{
		ACTION, PAUSED, POST_LEVEL
	};
	enum class PostLevelMenu
	{
		MAIN, SHOP_MAIN, SHOP_WEAPONS, SHOP_PROTECTION, SHOP_ENGINE, SHOP_GADGETS
	};

	class GameState : public AppState
	{
	public:
		GameState(Camera* cameraPtr, Starfield* starfieldPtr);
		void Init() override;
		void ProcessEvent(const SDL_Event& event) override;
		AppStateID Update(float dt) override;
		void Draw() override;

	private:
		void StartActionMode(bool startLevel = false);
		void StartPauseMenu();
		void StartPostLevelMenu(PostLevelMenu menu);

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
		Game m_game;
		GameMode m_mode;
		PostLevelMenu m_postLevelMenuMode;
		d2d::Menu m_menu;
		bool m_showFPS;

		// Gamepad input configuration
		Gamepad m_gamepad;
		Keyboard m_keyboard;
		PlayerController m_playerController;

		// Pause menu
		const std::string m_pauseTitle{ "Paused" };
		const std::string m_resumeString{ "Resume" };
		const std::string m_quitString{ "Quit" };

		// Post-level menu
		const std::string m_postLevelTitle{ "Would you like to visit the shop to purchase upgrades?" };
		const std::string m_nextLevelString{ "Start Next Wave" };
		const std::string m_purchaseString{ "Purchase Upgrades" };
		const std::vector<std::string> m_postLevelMenuLabels{ m_nextLevelString, m_purchaseString, m_quitString };

		// Shop menu
		const std::string m_shopTitle{ "Shop" };
		const std::string m_weaponsString{ "Weapons" };
		const std::string m_protectionString{ "Hull/Shields" };
		const std::string m_engineString{ "Engine Upgrades" };
		const std::string m_gadgetsString{ "Gadgets" };
		const std::string m_backString{ "Back" };

		// FPS display
		const b2Vec2 m_fpsPosition{ 1.0f, 1.0f };
		const d2d::AlignmentAnchor m_fpsAlignment{ d2d::AlignmentAnchorX::RIGHT, d2d::AlignmentAnchorY::TOP };
	};
}
