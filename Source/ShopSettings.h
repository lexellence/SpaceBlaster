/**************************************************************************************\
** File: ShopSettings.h
** Project:
** Author: David Leksen
** Date:
**
** Header file for shop settings
**
\**************************************************************************************/
#pragma once
#include "Shop.h"
namespace Space
{
	// Unique Item IDs
	enum ShopItemID : int
	{
		GUNS_2, GUNS_3, GUNS_4, GUNS_5,
		REPAIR_HULL, REFUEL
	};
	namespace ShopSettings
	{
		const std::string TITLE{ "Shop" };

		// Repairs
		//const float REPAIR_COST_PER_HP = 0.5f;
		//const std::string REPAIR_HULL_NAME{ "Basic hull repairs" };
		//const float REFUEL_COST_PER_FP = 0.5f;
		//const std::string REFUEL_NAME{ "Refuel" };

		// Weapon room
		const ShopItemProgression GUNS
		{
			{ "Dual Guns", 5.0f, GUNS_2},
			{ "Trip Guns", 10.0f, GUNS_3 },
			{ "Quad Guns", 15.0f, GUNS_4 },
			{ "Guns Blazin'", 20.0f, GUNS_5 }
		};
		//const ShopItemProgression LASERS
		//{
		//	{ "Laser", 10.0f },
		//	{ "Laser Cooler", 20.0f }
		//};
		//const ShopItemProgression MISSILES
		//{
		//	{ "Dumb Missiles", 10.0f },
		//	{ "Radar-Guided Missiles", 20.0f },
		//	{ "Space Blaster", 20.0f }
		//};
		const ShopRoom WEAPONS_ROOM{ 
			{ "Weapons" },
			{
				GUNS
				//LASERS,
				//MISSILES
			}
		};

		// Protection room
		//const ShopItemProgression HULL
		//{
		//	{ "Reinforced Hull", 20.0f },
		//	{ "Max-Strength Hull", 40.0f }
		//};
		//const ShopItemProgression SHIELDS
		//{
		//	{ "Energy Shield", 20.0f },
		//	{ "High-Power Energy Shield", 40.0f }
		//};
		//const ShopItemProgression SPINNERS
		//{
		//	{ "Spinner", 15.0f },
		//	{ "Dual Spinners", 25.0f },
		//	{ "Triple Spinners", 35.0f }
		//};
		//const ShopRoom PROTECTION_ROOM{ "Hull/Shields", {HULL, SHIELDS, SPINNERS} };

		// Engine room
		//const ShopItemProgression THRUSTERS
		//{
		//	{ "Dual thrusters", 15.0f },
		//	{ "Triple thrusters", 20.0f },
		//	{ "Quad thrusters", 25.0f },
		//	{ "Max thrusters", 30.0f }
		//};
		//const ShopItemProgression BRAKES
		//{
		//	{ "Space Brakes", 20.0f },
		//	{ "Nicer Space Brakes", 40.0f }
		//};
		//const ShopItemProgression BOOSTERS
		//{
		//	{ "Little Booster", 20.0f },
		//	{ "Big Booster", 40.0f }
		//};
		//const ShopItemProgression FUEL
		//{
		//	{ "Bigger Fuel Tank", 20.0f },
		//	{ "MAssive Fuel Tank", 40.0f }
		//};
		//const ShopItemProgression TURNING
		//{
		//	{ "Faster Turning", 20.0f },
		//	{ "Fastest Turning", 40.0f }
		//};
		//const ShopRoom ENGINE_ROOM{ "Engine Upgrades", {THRUSTERS, BRAKES, BOOSTERS, FUEL, TURNING} };

		// Gadgets room
		//const ShopItemProgression RADAR
		//{
		//	{ "Short-Range Radar", 20.0f },
		//	{ "Medium-Range Radar", 40.0f },
		//	{ "Long-Range Radar", 60.0f }
		//};
		//const ShopRoom GADGETS_ROOM{ "Gadgets", {RADAR} };

		// Shop
		const std::vector<ShopRoom> ROOM_LIST
		{ 
			WEAPONS_ROOM
			//PROTECTION_ROOM,
			//ENGINE_ROOM,
			//GADGETS_ROOM
		};
	}
}
