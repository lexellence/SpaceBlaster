/**************************************************************************************\
** File: App.h
** Project:
** Author: David Leksen
** Date:
**
** Header file for the App class
**
\**************************************************************************************/
#pragma once
#include "AppState.h"
#include "AppDef.h"
#include "IntroState.h"
#include "MainMenuState.h"
#include "GameState.h"
#include "Exceptions.h"
namespace Space
{
	const AppStateID FIRST_APP_STATE = AppStateID::INTRO;
	const float MAX_APP_STEP = 1.0f;

	class App
	{
	public:
		~App();
		void Run();
	private:
		void Init();
		void Step(float dt);

		std::shared_ptr<AppState> GetStatePtr(AppStateID appState);
		void InitCurrentState();
		void UpdateCurrentState(float dt);
		void Draw();
		void Shutdown();

		std::shared_ptr<IntroState> m_introStatePtr;
		std::shared_ptr<MainMenuState> m_mainMenuStatePtr;
		std::shared_ptr<GameState> m_gameStatePtr;

		AppStateID m_currentState{ FIRST_APP_STATE };
		AppStateID m_nextState{ FIRST_APP_STATE };

		bool m_hasFocus{ false };
		Camera m_camera;
		Starfield m_starfield;
	};
}
