/**************************************************************************************\
** File: StarfieldSettings.h
** Project:
** Author: David Leksen
** Date:
**
** Header file for starfield settings
**
\**************************************************************************************/
#pragma once
namespace Space
{
	namespace StarfieldSettings
	{
		const float EDGE_PADDING_FACTOR = 1.3f;
		const float DENSITY = 0.01f;
		const d2d::Range<float> SPEED_FACTOR_RANGE{ 0.0001f, 0.20f };
		const d2d::Range<int> POINT_SIZE_INDEX_RANGE{ 5, 10 };
		const int MAX_POINT_SIZE_INDEX_VARIATION = 0;
		const d2d::ColorRange COLOR_RANGE{
			{ 1.0f, 1.0f, 1.0f, 0.3f },
			{ 1.0f, 1.0f, 1.0f, 0.95f } };
		const float MAX_ALPHA_VARIATION = 0.01f;
	}
}
