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
		void InitStarfield();
		void Step(float dt);

		void StartState(AppStateID newState);
		AppStateID Update(float dt);
		void Draw();
		void Shutdown();

		std::shared_ptr<AppState> m_currentStatePtr{ nullptr };
		AppStateID m_currentStateID{ FIRST_APP_STATE };

		bool m_hasFocus{ false };
		Camera m_camera;
		Starfield m_starfield;
	};
}
