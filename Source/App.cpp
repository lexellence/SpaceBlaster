/**************************************************************************************\
** File: App.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the App class
**
\**************************************************************************************/
#include "pch.h"
#include "App.h"
#include "AppState.h"
#include "AppDef.h"
#include "IntroState.h"
#include "MainMenuState.h"
#include "GameState.h"
#include "Exceptions.h"
#include "CameraSettings.h"
#include "StarfieldSettings.h"

namespace Space
{
	void App::Run()
	{
		Init();

		d2d::Timer timer;
		timer.Start();
		while(m_currentStateID != AppStateID::QUIT)
		{
			timer.Update();
			Step(timer.Getdt());
		}
		Shutdown();
	}
	void App::Init()
	{
		// Init d2d
		d2d::Init(d2LogSeverityTrace, "Space.log");
		{
			AppDef settings;
			settings.LoadFrom("Data/app.hjson");

			d2d::InitGamepads(settings.gamepads);
			d2d::Window::Init(settings.window);
			m_hasFocus = false;
		}
		d2d::SeedRandomNumberGenerator();

		m_camera.Init(CameraSettings::DIMENSION_RANGE, CameraSettings::ZOOM_SPEED, 
			CameraSettings::INITIAL_ZOOM_OUT_PERCENT);
		InitStarfield();

		// Start first app state
		try
		{
			StartState(FIRST_APP_STATE);
			d2d::Window::StartScene();
			d2d::Window::EndScene();
		}
		catch(const GameException& e)
		{
			std::string message{ "AppState init error: "s + std::string{ e.what() } };
			d2d::Window::ShowSimpleMessageBox(d2d::MessageBoxType::D2D_ERROR, "Error"s, message);
			m_currentStateID = AppStateID::QUIT;
		}
	}
	void App::InitStarfield()
	{
		StarfieldDef def;
		def.edgePaddingFactor = StarfieldSettings::EDGE_PADDING_FACTOR;
		def.density = StarfieldSettings::DENSITY;
		def.speedFactorRange = StarfieldSettings::SPEED_FACTOR_RANGE;
		def.pointSizeIndexRange = StarfieldSettings::POINT_SIZE_INDEX_RANGE;
		def.maxPointSizeIndexVariation = StarfieldSettings::MAX_POINT_SIZE_INDEX_VARIATION;
		def.colorRange = StarfieldSettings::COLOR_RANGE;
		def.maxAlphaVariation = StarfieldSettings::MAX_ALPHA_VARIATION;
		m_starfield.Init(def, CameraSettings::DIMENSION_RANGE.GetMax());
	}
	void App::Step(float dt)
	{
		d2d::ClampHigh(dt, MAX_APP_STEP);

		AppStateID nextStateID = Update(dt);

		if(nextStateID != m_currentStateID)
			StartState(nextStateID);
		else if(m_currentStateID != AppStateID::QUIT)
			if(m_hasFocus)
				Draw();
	}
	App::~App()
	{
		// Just in case an exception brings us out of the main loop
		Shutdown();
	}
	void App::StartState(AppStateID newStateID)
	{
		try
		{
			m_currentStateID = newStateID;
			//AppState* lastStatePtr = m_currentStatePtr;
			switch(m_currentStateID)
			{
			case AppStateID::INTRO:		
				m_currentStatePtr = std::make_shared<IntroState>(&m_camera, &m_starfield); break;
			case AppStateID::MAIN_MENU: 
				m_currentStatePtr = std::make_shared<MainMenuState>(&m_camera, &m_starfield); break;
			case AppStateID::GAME:	
				m_currentStatePtr = std::make_shared<GameState>(&m_camera, &m_starfield); break;
			default: return;
			}
			m_currentStatePtr->Init();
		}
		catch(const GameException& e)
		{
			std::string message{ "AppState init error: "s + std::string{ e.what() } };
			d2d::Window::ShowSimpleMessageBox(d2d::MessageBoxType::D2D_ERROR, "Error"s, message);
			m_currentStateID = AppStateID::QUIT;
		}
	}
	AppStateID App::Update(float dt)
	{
		// Handle events
		SDL_Event event;
		while(SDL_PollEvent(&event) != 0)
		{
			switch(event.type)
			{
			case SDL_QUIT:
				return AppStateID::QUIT;
			case SDL_WINDOWEVENT:
				switch(event.window.event)
				{
				case SDL_WINDOWEVENT_FOCUS_LOST:
					m_hasFocus = false;
					break;
				case SDL_WINDOWEVENT_FOCUS_GAINED:
					m_hasFocus = true;
					break;
				case SDL_WINDOWEVENT_CLOSE:
					return AppStateID::QUIT;
				//case SDL_WINDOWEVENT_SIZE_CHANGED:
				//	d2d::Window::Resize(event.window.data1, event.window.data2);
				//	return;
				}
			}
			m_currentStatePtr->ProcessEvent(event);
		}

		if(m_hasFocus)
		{
			try
			{
				return m_currentStatePtr->Update(dt);
			}
			catch(const GameException& e)
			{
				std::string message{ "Game update error: "s + std::string{ e.what() } };
				d2d::Window::ShowSimpleMessageBox(d2d::MessageBoxType::D2D_ERROR, "Error"s, message);
				return AppStateID::QUIT;
			}
		}
		return m_currentStateID;
	}
	void App::Draw()
	{
		d2d::Window::StartScene();
		m_currentStatePtr->Draw();
		d2d::Window::EndScene();
	}
	void App::Shutdown()
	{
		d2d::Shutdown();
	}
}
