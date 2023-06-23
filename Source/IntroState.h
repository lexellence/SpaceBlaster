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

namespace Space
{
	class IntroState : public AppState
	{
	public:
		IntroState(Camera* cameraPtr, Starfield* starfieldPtr);
		void Init() override;
		void ProcessEvent(const SDL_Event& event) override;
		AppStateID Update(float dt) override;
		void Draw() override;

	private:
		d2d::FontReference m_alexBrushFont{ "Fonts/AlexBrush.otf" };
		d2d::FontReference m_orbitronLightFont{ "Fonts/OrbitronLight.otf" };

		const std::string m_title{ "Space Blaster" };
		const d2d::AlignmentAnchor m_titleAnchor{ d2d::AlignmentAnchorX::CENTER, d2d::AlignmentAnchorY::BOTTOM };
		const d2d::TextStyle m_titleTextStyle{
			&m_orbitronLightFont,
			{ 0.0f, 1.0f, 0.0f, 1.0f },
			0.045f
		};

		const std::string m_author{ "Lexellence Games" };
		const d2d::AlignmentAnchor m_authorAlignment{ d2d::AlignmentAnchorX::CENTER, d2d::AlignmentAnchorY::TOP };
		d2d::TextStyle m_authorTextStyle{
			&m_alexBrushFont,
			d2d::Color(),
			0.045f
		};

		const float m_titleAccelerationScreensPerSecondSquared{ 0.15f };
		const float m_titleStartScreenY{ 1.0f };
		const float m_titleFinalScreenY{ 0.5f };
		const float m_authorFadeDelay{ 0.6f };
		const float m_authorFadesPerSecond{ 0.4f };
		const float m_authorStartAlpha{ 0.0f };
		const float m_authorFinalAlpha{ 1.0f };
		const float m_fadeTolerance{ 0.001f };
		const b2Vec2 m_authorPosition{ 0.5f, 0.4f };

		bool m_gotoMenu;
		bool m_animationsComplete;
		bool m_titleFalling;
		b2Vec2 m_titlePosition;
		float m_titleSpeedScreensPerSecond;
		bool m_authorFadingIn;
		float m_authorFadeDelayElapsed;
	};
}
