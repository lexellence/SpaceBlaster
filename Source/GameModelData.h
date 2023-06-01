/**************************************************************************************\
** File: GameModelData.h
** Project:
** Author: David Leksen
** Date:
**
** Header file for model data (stuff that's not likely to need tweaking)
**
\**************************************************************************************/
#pragma once
namespace Space
{
	constexpr unsigned NUM_XLARGE_ASTEROID_MODELS{4};
	constexpr unsigned NUM_LARGE_ASTEROID_MODELS{2};
	constexpr unsigned NUM_MEDIUM_ASTEROID_MODELS{2};
	constexpr unsigned NUM_SMALL_ASTEROID_MODELS{2};
	constexpr unsigned NUM_ICON_MODELS{28};

	constexpr std::array<float, NUM_XLARGE_ASTEROID_MODELS>
		XLARGE_ASTEROID_RELATIVE_HEIGHTS{84.0f / 90.0f, 98.0f / 90.0f, 82.0f / 90.0f, 96.0f / 90.0f};
	constexpr std::array<float, NUM_LARGE_ASTEROID_MODELS>
		LARGE_ASTEROID_RELATIVE_HEIGHTS{43.0f / 41.5f, 40.0f / 41.5f};
	constexpr std::array<float, NUM_MEDIUM_ASTEROID_MODELS>
		MEDIUM_ASTEROID_RELATIVE_HEIGHTS{28.0f / 27.0f, 26.0f / 27.0f};
	constexpr std::array<float, NUM_SMALL_ASTEROID_MODELS>
		SMALL_ASTEROID_RELATIVE_HEIGHTS{18.0f / 16.5f, 15.0f / 16.5f};

	constexpr float SCOUT_TO_BLASTER_HEIGHT_RATIO{58.0f / 109.0f};
	constexpr float FAT_MISSILE_TO_MISSILE_HEIGHT_RATIO{8.0f / 7.0f};
	constexpr float LARGE_TO_XLARGE_ASTEROID_AVERAGE_HEIGHT_RATIO{41.5f / 90.0f};
	constexpr float MEDIUM_TO_XLARGE_ASTEROID_AVERAGE_HEIGHT_RATIO{27.0f / 90.0f};
	constexpr float SMALL_TO_XLARGE_ASTEROID_AVERAGE_HEIGHT_RATIO{16.5f / 90.0f};

	const d2d::ColorRange BLASTER_PARTICLE_COLOR_RANGE{{39, 59, 39, 255}, {87, 119, 87, 255}};
	const d2d::ColorRange ASTEROID_PARTICLE_COLOR_RANGE{{92, 68, 52, 255}, {153, 112, 85, 255}};
	const d2d::ColorRange ROCK_PARTICLE_COLOR_RANGE{{115, 126, 131, 255}, {154, 170, 177, 255}};
}
