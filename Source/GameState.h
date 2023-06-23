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

		// Text
		d2d::FontReference m_orbitronLightFont{ "Fonts/OrbitronLight.otf"s };
		const d2d::TextStyle m_buttonTextStyle{
			&m_orbitronLightFont,
			{ 0.0f, 0.5f, 0.8f, 1.0f },
				0.035f
		};
		const d2d::TextStyle m_titleTextStyle{
			&m_orbitronLightFont,
			{ 0.8f, 0.8f, 0.8f, 0.8f },
				0.045f
		};

		// Menu colors
		const d2d::Color m_buttonColor{ d2d::COLOR_ZERO };
		const d2d::Color m_buttonHighlightColor{ 1.0f, 1.0f, 1.0f, 0.3f };
		const d2d::Color m_buttonBorderColor{ 0.5f, 0.5f, 0.5f, 0.5f };
		const d2d::Color m_postLevelBackgroundColor{ 0.1f, 0.1f, 0.1f, 0.6f };
		const d2d::Color m_pauseBackgroundColor{ 0.5f, 0.5f, 0.5f, 0.3f };

		// Pause menu
		const std::string m_pauseTitle{ "Paused" };
		const std::string m_resumeString{ "Resume" };
		const std::string m_quitString{ "Quit" };
		const std::vector<std::string> m_pauseMenuLabels{ m_resumeString, m_quitString };

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
		const std::vector<std::string> m_shopMenuLabels
		{ m_backString, m_weaponsString, m_protectionString, m_engineString, m_gadgetsString };
		const std::vector<std::string> m_weaponsMenuLabels{ m_backString };
		const std::vector<std::string> m_protectionMenuLabels{ m_backString };
		const std::vector<std::string> m_engineMenuLabels{ m_backString };
		const std::vector<std::string> m_gadgetsMenuLabels{ m_backString };

		// Menu setting helpers
		 d2d::MenuButton m_newButton{.textStyle{ m_buttonTextStyle },
			 .color{ m_buttonColor },
			 .highlightColor{ m_buttonHighlightColor },
			 .borderColor{ m_buttonBorderColor }
		};
		void SetMenuButtons(const std::vector<std::string>& labelList);

		// FPS display
		const b2Vec2 m_fpsPosition{ 1.0f, 1.0f };
		const d2d::AlignmentAnchor m_fpsAlignment{ d2d::AlignmentAnchorX::RIGHT, d2d::AlignmentAnchorY::TOP };
		const d2d::TextStyle m_fpsTextStyle{
			&m_orbitronLightFont,
			{ 1.0f, 1.0f, 0.0f, 1.0f },
				0.05f
		};
	};
}
