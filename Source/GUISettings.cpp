/**************************************************************************************\
** File: GUISettings.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for GUI settings
**
\**************************************************************************************/
#include "pch.h"
#include "GUISettings.h"
namespace Space::GUISettings::Menu
{
	bool IsGoBackEvent(const SDL_Event& event)
	{
		return (event.type == SDL_KEYDOWN && event.key.keysym.sym == BACK_KEY) ||
			(event.type == SDL_CONTROLLERBUTTONDOWN && event.cbutton.button == BACK_BUTTON);
	}
}