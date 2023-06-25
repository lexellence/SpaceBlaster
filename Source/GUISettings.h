/**************************************************************************************\
** File: GUISettings.h
** Project:
** Author: David Leksen
** Date:
**
** Header file for camera settings
**
\**************************************************************************************/
#pragma once
namespace Space
{
	namespace GUISettings
	{
		const d2d::Color normalButtonColor{ 0.0f, 0.3f, 0.0f, 0.2f };
		const d2d::Color backButtonColor{ 0.3f, 0.0f, 0.0f, 0.2f };

		const d2d::Color normalButtonHighlightColor{ 0.0f, 1.0f, 0.0f, 0.4f };
		const d2d::Color backButtonHighlightColor{ 1.0f, 0.0f, 0.0f, 0.4f };

		const d2d::Color normalButtonBorderColor{ 0.1f, 0.5f, 0.1f, 0.7f };
		const d2d::Color backButtonBorderColor{ 0.5f, 0.1f, 0.1f, 0.7f };

		const d2d::Color postLevelMenuBackgroundColor{ 0.1f, 0.1f, 0.1f, 0.6f };
		const d2d::Color pauseMenuBackgroundColor{ 0.5f, 0.5f, 0.5f, 0.3f };

		const float buttonTextSize = 0.035f;
		const d2d::FontReference orbitronLightFont{ "Fonts/OrbitronLight.otf" };
		const d2d::TextStyle menuTitleTextStyle{ &orbitronLightFont,
			{ 0.0f, 1.0f, 0.0f, 1.0f }, 0.045f };
		const d2d::TextStyle menuSubtitleTextStyle{ &orbitronLightFont,
			{ 0.0f, 0.0f, 1.0f, 1.0f }, buttonTextSize };
		const d2d::TextStyle normalButtonTextStyle{ &orbitronLightFont,
			{ 0.2f, 0.8f, 0.2f, 1.0f }, buttonTextSize };
		const d2d::TextStyle backButtonTextStyle{ &orbitronLightFont,
			{ 0.8f, 0.2f, 0.2f, 1.0f }, buttonTextSize };

		const d2d::ButtonStyle normalButtonStyle{
			.text{ normalButtonTextStyle },
			.color{ normalButtonColor },
			.highlightColor{ normalButtonHighlightColor },
			.borderColor{ normalButtonBorderColor }
		};
		const d2d::ButtonStyle backButtonStyle{
			.text{ backButtonTextStyle },
			.color{ backButtonColor },
			.highlightColor{ backButtonHighlightColor },
			.borderColor{ backButtonBorderColor }
		};

		const d2d::TextStyle fpsTextStyle{ &orbitronLightFont,
			{ 1.0f, 1.0f, 0.0f, 1.0f }, 0.05f };

	}
}
