/**************************************************************************************\
** File: Intro.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the Intro class
**
\**************************************************************************************/
#include "pch.h"
#include "Intro.h"
#include "AppState.h"

namespace Space
{
	void Intro::Init()
	{
		m_gotoMenu = false;
		m_animationsComplete = false;
		m_titleFalling = true;
		m_titlePosition.Set(0.5f, m_titleStartScreenY);
		m_titleSpeedScreensPerSecond = 0.0f;
		m_authorFadingIn = false;
		m_authorTextStyle.color.SetFloat(0.5f, 0.0f, 0.7f, m_authorStartAlpha);
		m_authorFadeDelayElapsed = 0.0f;
	}
	void Intro::ProcessEvent(const SDL_Event& event)
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
				m_authorTextStyle.color.alpha = m_authorFinalAlpha;
				m_animationsComplete = true;
			}
		}
	}
	AppStateID Intro::Update(float dt)
	{
		if(m_gotoMenu)
			return AppStateID::MAIN_MENU;

		// Update intro animations
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
					m_authorTextStyle.color.alpha += m_authorFadesPerSecond * dt;

					// If the text's alpha has reached it's destination within the fade tolerance
					if((m_authorTextStyle.color.alpha + m_fadeTolerance) > m_authorFinalAlpha)
					{
						// Set text's final destination
						m_authorTextStyle.color.alpha = m_authorFinalAlpha;

						// End animation
						m_authorFadingIn = false;
						m_animationsComplete = true;
					}
				}
			}
		}
		return AppStateID::INTRO;
	}
	void Intro::Draw()
	{
		if(m_gotoMenu)
			return;

		//d2d::Window::SetShowCursor(true);
		d2d::Window::DisableTextures();
		d2d::Window::EnableBlending();

		// Set camera to screen resolution
		b2Vec2 resolution{ d2d::Window::GetScreenResolution() };
		d2d::Window::SetCameraRect({ b2Vec2_zero, resolution });

		// Draw title
		d2d::Window::SetColor(m_titleTextStyle.color);
		d2d::Window::PushMatrix();
		d2d::Window::Translate(m_titlePosition * resolution);
		d2d::Window::DrawString(m_title, m_titleAlignment, m_titleTextStyle.size * resolution.x, m_titleTextStyle.font);
		d2d::Window::PopMatrix();

		// Draw author
		d2d::Window::SetColor(m_authorTextStyle.color);
		d2d::Window::PushMatrix();
		d2d::Window::Translate(m_authorPosition * resolution);
		d2d::Window::DrawString(m_author, m_authorAlignment, m_authorTextStyle.size * resolution.x, m_authorTextStyle.font);
		d2d::Window::PopMatrix();
	}
}