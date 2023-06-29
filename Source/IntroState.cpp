/**************************************************************************************\
** File: IntroState.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the IntroState class
**
\**************************************************************************************/
#include "pch.h"
#include "IntroState.h"
#include "AppState.h"
#include "Starfield.h"
#include "Camera.h"
#include "GUISettings.h"
#include "GUIStrings.h"
namespace Space
{
	void IntroState::Init()
	{
		m_gotoMenu = false;
		m_animationsComplete = false;
		m_titleFalling = true;
		m_titlePosition.Set(0.5f, m_titleStartScreenY);
		m_titleSpeedScreensPerSecond = 0.0f;
		m_authorFadingIn = false;
		m_authorFadeDelayElapsed = 0.0f;
		m_authorPercentFadedIn = 0.0f;
	}
	void IntroState::ProcessEvent(const SDL_Event& event)
	{
		if(event.type == SDL_KEYDOWN ||
			event.type == SDL_CONTROLLERBUTTONDOWN ||
			event.type == SDL_MOUSEBUTTONDOWN)
		{
			if(m_animationsComplete)
				m_gotoMenu = true;
			else
			{
				// Hurry animations on any input
				m_titleFalling = false;
				m_titlePosition.y = m_titleFinalScreenY;

				m_authorFadingIn = false;
				m_authorPercentFadedIn = 1.0f;
				m_animationsComplete = true;
			}
		}
	}
	AppStateID IntroState::Update(float dt)
	{
		if(m_gotoMenu)
			return AppStateID::MAIN_MENU;

		// Starfield
		m_cameraPtr->SetPosition(m_cameraPtr->GetPosition() - (dt * STARFIELD_DEFAULT_VELOCITY));
		m_starfieldPtr->Update(m_cameraPtr->GetPosition());

		// Intro animations
		if(!m_animationsComplete)
		{
			if(m_titleFalling)
			{
				// Move text
				m_titleSpeedScreensPerSecond += m_titleAccelerationScreensPerSecondSquared * dt;
				m_titlePosition.y -= m_titleSpeedScreensPerSecond * dt;

				// If the text has reached it's destination or beyond
				if(m_titlePosition.y <= m_titleFinalScreenY)
				{
					// In case it went too far, set it's final destination.
					m_titlePosition.y = m_titleFinalScreenY;

					// Goto next animation
					m_titleFalling = false;
					m_authorFadingIn = true;
				}
			}
			if(m_authorFadingIn)
			{
				if(m_authorFadeDelayElapsed < m_authorFadeDelay)
					m_authorFadeDelayElapsed += dt;
				if(m_authorFadeDelayElapsed >= m_authorFadeDelay)
				{
					// Fade text in
					m_authorPercentFadedIn += m_authorFadesPerSecond * dt;
					if(m_authorPercentFadedIn >= 1.0f)
					{
						// Set text's final destination
						m_authorPercentFadedIn = 1.0f;

						// End animation
						m_authorFadingIn = false;
						m_animationsComplete = true;
					}
				}
			}
		}
		return AppStateID::INTRO;
	}
	void IntroState::Draw()
	{
		if(m_gotoMenu)
			return;

		//d2d::Window::SetShowCursor(true);
		d2d::Window::DisableTextures();
		d2d::Window::EnableBlending();

		// Starfield
		d2d::Window::SetCameraRect(m_cameraPtr->GetRect());
		m_starfieldPtr->Draw();

		// Set camera to screen resolution
		b2Vec2 resolution{ d2d::Window::GetScreenResolution() };
		d2d::Window::SetCameraRect({ b2Vec2_zero, resolution });

		// Draw title
		d2d::Window::SetColor(GUISettings::Intro::Text::Color::TITLE);
		d2d::Window::PushMatrix();
		d2d::Window::Translate(m_titlePosition * resolution);
		d2d::Window::DrawString(m_titleFont, GUIStrings::Intro::TITLE, 
			GUISettings::Intro::Text::Size::TITLE * resolution.y, m_titleAnchor);
		d2d::Window::PopMatrix();

		// Draw author
		d2d::Color authorColor = GUISettings::Intro::Text::Color::SUBTITLE;
		if(m_authorFadingIn)
			authorColor.alpha *= m_authorPercentFadedIn;
		d2d::Window::SetColor(authorColor);
		d2d::Window::PushMatrix();
		d2d::Window::Translate(m_authorPosition * resolution);
		d2d::Window::DrawString(m_authorFont, GUIStrings::Intro::AUTHOR, 
			GUISettings::Intro::Text::Size::SUBTITLE * resolution.y, m_authorAlignment);
		d2d::Window::PopMatrix();
	}
}