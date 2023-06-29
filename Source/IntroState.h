/**************************************************************************************\
** File: IntroState.h
** Project:
** Author: David Leksen
** Date:
**
** Header file for the IntroState class
**
\**************************************************************************************/
#pragma once
#include "AppState.h"
#include "GUISettings.h"
namespace Space
{
	class IntroState : public AppState
	{
	public:
		using AppState::AppState;
		void Init() override;
		void ProcessEvent(const SDL_Event& event) override;
		AppStateID Update(float dt) override;
		void Draw() override;

	private:
		d2d::FontReference m_titleFont{ GUISettings::Intro::Text::Font::TITLE };
		d2d::FontReference m_authorFont{ GUISettings::Intro::Text::Font::SUBTITLE };

		const d2d::AlignmentAnchor m_titleAnchor{
			d2d::AlignmentAnchorX::CENTER, d2d::AlignmentAnchorY::BOTTOM };
		const d2d::AlignmentAnchor m_authorAlignment{
			d2d::AlignmentAnchorX::CENTER, d2d::AlignmentAnchorY::TOP };

		const float m_titleAccelerationScreensPerSecondSquared{ 0.15f };
		const float m_titleStartScreenY{ 1.0f };
		const float m_titleFinalScreenY{ 0.5f };
		const float m_authorFadeDelay{ 0.6f };
		const float m_authorFadesPerSecond{ 0.4f };
		const b2Vec2 m_authorPosition{ 0.5f, 0.4f };

		bool m_gotoMenu;
		bool m_animationsComplete;
		bool m_titleFalling;
		b2Vec2 m_titlePosition;
		float m_titleSpeedScreensPerSecond;
		bool m_authorFadingIn;
		float m_authorFadeDelayElapsed;
		float m_authorPercentFadedIn;
	};
}