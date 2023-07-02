/**************************************************************************************\
** File: GUISettings.h
** Project:
** Author: David Leksen
** Date:
**
** Header file for GUI settings
**
\**************************************************************************************/
#pragma once
namespace Space::GUISettings {
	namespace Intro::Text {
		namespace Color {
			const d2d::Color TITLE{ 0.0f, 1.0f, 0.0f, 1.0f };
			const d2d::Color SUBTITLE{ 0.5f, 0.0f, 0.7f, 1.0f };
		}
		namespace Size {
			const float TITLE = 0.075f;
			const float SUBTITLE = 0.06f;
		}
		namespace Font {
			const std::string TITLE = "Fonts/OrbitronLight.otf";
			const std::string SUBTITLE = "Fonts/AlexBrush.otf";
		}
	}
	namespace Menu {
		const SDL_Keycode BACK_KEY = SDLK_ESCAPE;
		const SDL_GameControllerButton BACK_BUTTON = SDL_CONTROLLER_BUTTON_B;
		bool IsGoBackEvent(const SDL_Event& event);
	}
	namespace Menu::Text {
		namespace Color {
			const d2d::Color TITLE{ 0.0f, 1.0f, 0.0f, 1.0f };
			const d2d::Color SUBTITLE{ 0.0f, 0.0f, 1.0f, 1.0f };
		}
		namespace Size {
			const float TITLE = 0.045f;
			const float SUBTITLE = 0.035f;
			const float BUTTON = 0.035f;
		}
		namespace Font {
			const std::string TITLE = "Fonts/OrbitronLight.otf";
			const std::string SUBTITLE = "Fonts/OrbitronLight.otf";
			const std::string BUTTON = "Fonts/OrbitronLight.otf";
		}
	}
	namespace Menu::BackgroundColor {
		const d2d::Color MAIN{};
		const d2d::Color POST_LEVEL{ 0.1f, 0.1f, 0.1f, 0.6f };
		const d2d::Color PAUSE{ 0.5f, 0.5f, 0.5f, 0.3f };
	}
	namespace Menu::ButtonStyles {
		namespace Normal {
			const d2d::ButtonStyle PRIMARY {
				.textColor{ 0.2f, 0.8f, 0.2f, 1.0f },
					.backgroundColor{ 0.0f, 0.3f, 0.0f, 0.3f },
					.borderColor{ 0.1f, 0.5f, 0.1f, 1.0f }
			};
			const d2d::ButtonStyle SECONDARY {
				.textColor{ 0.2f, 0.2f, 0.8f, 1.0f },
					.backgroundColor{ 0.0f, 0.0f, 0.3f, 0.3f },
					.borderColor{ 0.1f, 0.1f, 0.5f, 1.0f },
			};
			const d2d::ButtonStyle BACK {
				.textColor{ 0.8f, 0.2f, 0.2f, 1.0f },
					.backgroundColor{ 0.3f, 0.0f, 0.0f, 0.3f },
					.borderColor{ 0.5f, 0.1f, 0.1f, 1.0f }
			};
			const d2d::ButtonStyle GRAYED {
				.textColor{ 0.3f, 0.3f, 0.3f, 1.0f },
					.backgroundColor{ 0.1f, 0.1f, 0.1f, 0.3f },
					.borderColor{ 0.25f, 0.25f, 0.25f, 1.0f }
			};
		}
		namespace Highlight {
			const d2d::ButtonStyle PRIMARY {
				.textColor{ 0.0f, 1.0f, 0.0f, 1.0f },
					.backgroundColor{ 0.0f, 0.7f, 0.0f, 0.4f },
					.borderColor{ 0.0f, 1.0f, 0.0f, 1.0f }
			};
			const d2d::ButtonStyle SECONDARY {
				.textColor{ 0.0f, 0.0f, 1.0f, 1.0f },
					.backgroundColor{ 0.0f, 0.0f, 0.7f, 0.4f },
					.borderColor{ 0.0f, 0.0f, 1.0f, 1.0f },
			};
			const d2d::ButtonStyle BACK	{
				.textColor{ 1.0f, 0.0f, 0.0f, 1.0f },
					.backgroundColor{ 0.7f, 0.0f, 0.0f, 0.4f },
					.borderColor{ 1.0f, 0.0f, 0.0f, 1.0f }
			};
			const d2d::ButtonStyle GRAYED {
				.textColor{ 0.6f, 0.6f, 0.6f, 1.0f },
					.backgroundColor{ 0.2f, 0.2f, 0.2f, 0.4f },
					.borderColor{ 0.6f, 0.6f, 0.6f, 1.0f }
			};
		}
	}
	namespace HUD::ViewSections {
		const d2d::Rect ACTION{{ 0.1f, 0.0f }, { 0.9f, 1.0f }};
		const d2d::Rect LEFT{{ 0.0f, 0.0f }, { ACTION.lowerBound.x, 1.0f }};
		const d2d::Rect RIGHT{{ ACTION.upperBound.x, 0.0f }, { 1.0f, 1.0f }};
	}
	namespace HUD::Background {
		namespace Color {
			const d2d::Color DEFAULT{ 0.0f, 0.3f, 0.0f, 0.3f };
		}
	}
	namespace HUD::Text {
		namespace Color {
			const d2d::Color FPS{ 1.0f, 1.0f, 0.0f, 1.0f };
			const d2d::Color FUEL{ 1.0f, 0.2f, 0.2f, 1.0f };
			const d2d::Color CREDITS{ 0.2f, 0.2f, 1.0f, 1.0f };
			const d2d::Color LEVEL{1.0f, 1.0f, 0.2f, 1.0f};
			const d2d::Color ICONS{1.0f, 0.2f, 1.0f, 1.0f};
		}
		namespace Size {
			const float DEFAULT = 0.035f;
			const float FPS = DEFAULT;
			const float FUEL = DEFAULT;
			const float CREDITS = DEFAULT;
			const float LEVEL = DEFAULT;
			const float ICONS = DEFAULT;
		}
		namespace Position {
			const b2Vec2 FPS{ 0.99f, 0.99f };
			const d2d::AlignmentAnchor FPS_ALIGNMENT{ d2d::AlignmentAnchorX::RIGHT, d2d::AlignmentAnchorY::TOP };
			const b2Vec2 FUEL{ HUD::ViewSections::LEFT.GetCenterX(), 0.25f};
			const d2d::AlignmentAnchor FUEL_ALIGNMENT{ d2d::AlignmentAnchorX::CENTER, d2d::AlignmentAnchorY::CENTER };
			const b2Vec2 CREDITS{ HUD::ViewSections::LEFT.GetCenterX(), 0.5f };
			const d2d::AlignmentAnchor CREDITS_ALIGNMENT{ d2d::AlignmentAnchorX::CENTER, d2d::AlignmentAnchorY::CENTER };
			const b2Vec2 LEVEL{ HUD::ViewSections::LEFT.GetCenterX(), 0.75f };
			const d2d::AlignmentAnchor LEVEL_ALIGNMENT{ d2d::AlignmentAnchorX::CENTER, d2d::AlignmentAnchorY::CENTER };
			const b2Vec2 ICONS{ HUD::ViewSections::RIGHT.GetCenterX(), 0.75f };
			const d2d::AlignmentAnchor ICONS_ALIGNMENT{ d2d::AlignmentAnchorX::CENTER, d2d::AlignmentAnchorY::CENTER };
		}
		namespace Font {
			const std::string DEFAULT = "Fonts/OrbitronLight.otf";
		}
	}
}
