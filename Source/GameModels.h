/**************************************************************************************\
** File: GameModels.h
** Project:
** Author: David Leksen
** Date:
**
** Header file for GameModels
**
\**************************************************************************************/
#pragma once
#include "GameSettings.h"
namespace Space
{
	constexpr unsigned NUM_XLARGE_ASTEROID_MODELS = 4;
	constexpr unsigned NUM_LARGE_ASTEROID_MODELS = 2;
	constexpr unsigned NUM_MEDIUM_ASTEROID_MODELS = 2;
	constexpr unsigned NUM_SMALL_ASTEROID_MODELS = 2;
	constexpr unsigned NUM_ICON_MODELS = 28;

	constexpr std::array<float, NUM_XLARGE_ASTEROID_MODELS>
		XLARGE_ASTEROID_RELATIVE_HEIGHTS{84.0f / 90.0f, 98.0f / 90.0f, 82.0f / 90.0f, 96.0f / 90.0f};
	constexpr std::array<float, NUM_LARGE_ASTEROID_MODELS>
		LARGE_ASTEROID_RELATIVE_HEIGHTS{43.0f / 41.5f, 40.0f / 41.5f};
	constexpr std::array<float, NUM_MEDIUM_ASTEROID_MODELS>
		MEDIUM_ASTEROID_RELATIVE_HEIGHTS{28.0f / 27.0f, 26.0f / 27.0f};
	constexpr std::array<float, NUM_SMALL_ASTEROID_MODELS>
		SMALL_ASTEROID_RELATIVE_HEIGHTS{18.0f / 16.5f, 15.0f / 16.5f};

	//+---------------------------\-------------------------------
	//|		   GameModels 		  |
	//\---------------------------/-------------------------------
	struct GameModels
	{
		//+---------------------------\-------------------------------
		//|		    Textures		  |
		//\---------------------------/-------------------------------
		struct Textures
		{
			// Big ships
			d2d::TextureAtlas bigShipAtlas{ "Textures/bigships.png"s, "Textures/bigships.xml"s};
			d2d::TextureFromAtlas blaster{ bigShipAtlas, "ship001"s};
			d2d::TextureFromAtlas scout{ bigShipAtlas, "ship002"s};

			// Tiny ships
			d2d::TextureAtlas tinyShipAtlas{ "Textures/tinyships.png"s, "Textures/tinyships.xml"s};
			d2d::TextureFromAtlas ufoGreen{ tinyShipAtlas, "tinyship015green"s};
			d2d::TextureFromAtlas ufoGray{ tinyShipAtlas, "tinyship015gray"s};

			// Effects
			d2d::TextureAtlas effectAtlas{ "Textures/effects.png"s, "Textures/effects.xml"s};
			d2d::TextureFromAtlas thruster{ effectAtlas, "thruster1"s};

			// Items
			d2d::TextureAtlas itemAtlas{ "Textures/items.png"s, "Textures/items.xml"s};
			d2d::TextureFromAtlas apple{ itemAtlas, "apple"s};
			d2d::TextureFromAtlas bumper{ itemAtlas, "repulser1"s};
			d2d::TextureFromAtlas soda{ itemAtlas, "sodacan"s};
			d2d::TextureFromAtlas melon{ itemAtlas, "watermelon"s};

			// Icons
			d2d::TextureAtlas iconAtlas{ "Textures/icons.png"s, "Textures/icons.xml"s };
			std::array<d2d::TextureFromAtlas, NUM_ICON_MODELS> icons{{
				{ iconAtlas, "Solitaire"s },
				{ iconAtlas, "RegistryDocument"s },
				{ iconAtlas, "OutlookExpress"s },
				{ iconAtlas, "NetworkSetup"s },
				{ iconAtlas, "MyDocuments"s },
				{ iconAtlas, "MyComputer"s },
				{ iconAtlas, "Mouse"s },
				{ iconAtlas, "MediaCenterFile"s },
				{ iconAtlas, "GenericVideo"s },
				{ iconAtlas, "GenericTextDocument"s },
				{ iconAtlas, "GenericMedia"s },
				{ iconAtlas, "GenericDocument"s },
				{ iconAtlas, "GenericAudio"s },
				{ iconAtlas, "GameController"s },
				{ iconAtlas, "Font"s },
				{ iconAtlas, "FolderClosed"s },
				{ iconAtlas, "FloppyDisk"s },
				{ iconAtlas, "EntireNetwork"s },
				{ iconAtlas, "DVD-RW"s },
				{ iconAtlas, "DLL"s },
				{ iconAtlas, "DiskDefragmenter"s },
				{ iconAtlas, "CommandPrompt"s },
				{ iconAtlas, "ChangeOrRemovePrograms"s },
				{ iconAtlas, "Calculator"s },
				{ iconAtlas, "BluetoothDevices"s },
				{ iconAtlas, "BAT"s },
				{ iconAtlas, "IE6"s },
				{ iconAtlas, "GIF"s }
				}};

			// Asteroids
			d2d::TextureAtlas asteroidAtlas{ "Textures/asteroids.png"s, "Textures/asteroids.xml"s };
			std::array<d2d::TextureFromAtlas, NUM_XLARGE_ASTEROID_MODELS> asteroidsXLarge{{
				{ asteroidAtlas, "asteroidxlarge1"s },
				{ asteroidAtlas, "asteroidxlarge2"s },
				{ asteroidAtlas, "asteroidxlarge3"s },
				{ asteroidAtlas, "asteroidxlarge4"s } }};
			std::array<d2d::TextureFromAtlas, NUM_XLARGE_ASTEROID_MODELS> rocksXLarge{{
				{ asteroidAtlas, "rockxlarge1"s },
				{ asteroidAtlas, "rockxlarge2"s },
				{ asteroidAtlas, "rockxlarge3"s },
				{ asteroidAtlas, "rockxlarge4"s } }};
			std::array<d2d::TextureFromAtlas, NUM_LARGE_ASTEROID_MODELS> asteroidsLarge{{
				{ asteroidAtlas, "asteroidlarge1"s },
				{ asteroidAtlas, "asteroidlarge2"s } }};
			std::array<d2d::TextureFromAtlas, NUM_LARGE_ASTEROID_MODELS> rocksLarge{{
				{ asteroidAtlas, "rocklarge1"s },
				{ asteroidAtlas, "rocklarge2"s } }};
			std::array<d2d::TextureFromAtlas, NUM_MEDIUM_ASTEROID_MODELS> asteroidsMedium{{
				{ asteroidAtlas, "asteroidmedium1"s },
				{ asteroidAtlas, "asteroidmedium2"s } }};
			std::array<d2d::TextureFromAtlas, NUM_MEDIUM_ASTEROID_MODELS> rocksMedium{{
				{ asteroidAtlas, "rockmedium1"s },
				{ asteroidAtlas, "rockmedium2"s } }};
			std::array<d2d::TextureFromAtlas, NUM_SMALL_ASTEROID_MODELS> asteroidsSmall{{
				{ asteroidAtlas, "asteroidsmall1"s },
				{ asteroidAtlas, "asteroidsmall2"s } }};
			std::array<d2d::TextureFromAtlas, NUM_SMALL_ASTEROID_MODELS> rocksSmall{{
				{ asteroidAtlas, "rocksmall1"s },
				{ asteroidAtlas, "rocksmall2"s } }};

			// Projectiles
			d2d::TextureAtlas projectileAtlas{ "Textures/projectiles.png"s, "Textures/projectiles.xml"s };
			d2d::TextureFromAtlas bullet{ projectileAtlas, "fireball1"s};
			std::array<d2d::TextureFromAtlas, 2> missileFrames{{
				{ projectileAtlas, "rocket005a"s },
				{ projectileAtlas, "rocket005b"s } }};
			std::array<d2d::TextureFromAtlas, 2> fatMissileFrames{{
				{ projectileAtlas, "rocket006a"s },
				{ projectileAtlas, "rocket006b"s } }};
		} textures;

		//+---------------------------\-------------------------------
		//|		     Models 		  |
		//\---------------------------/-------------------------------
		// Ships
		Model blaster{ "ship001", {textures.blaster} };
		Model scout{ "ship002", {textures.scout} };
		Model ufoGreen{ "tinyship015green", {textures.ufoGreen} };
		Model ufoGray{ "tinyship015gray", {textures.ufoGray} };
		Model blasterThruster{ "thruster1", {{textures.thruster, 0.0f, d2d::WHITE_OPAQUE, BLASTER_THRUSTER_RELATIVE_SIZE}} };
		Model scoutThruster{ "thruster1", {{textures.thruster, 0.0f, d2d::WHITE_OPAQUE, SCOUT_THRUSTER_RELATIVE_SIZE}} };

		// Items
		Model bumper{ "repulser1", {textures.bumper} };
		Model soda{ "sodacan", {textures.soda} };
		Model melon{ "watermelon", {textures.melon} };
		Model apple{ "apple", {textures.apple} };

		// XLarge
		std::array<Model, NUM_XLARGE_ASTEROID_MODELS>
			asteroidsXLarge{
			Model{ "asteroidxlarge1", { textures.asteroidsXLarge.at(0) } },
				Model{ "asteroidxlarge2", {textures.asteroidsXLarge.at(1)} },
				Model{ "asteroidxlarge3", {textures.asteroidsXLarge.at(2)} },
				Model{ "asteroidxlarge4", {textures.asteroidsXLarge.at(3)} } };
		std::array<Model, NUM_XLARGE_ASTEROID_MODELS>
			rocksXLarge{
			Model{ "asteroidxlarge1", { textures.rocksXLarge.at(0) } },
				Model{ "asteroidxlarge2", {textures.rocksXLarge.at(1)} },
				Model{ "asteroidxlarge3", {textures.rocksXLarge.at(2)} },
				Model{ "asteroidxlarge4", {textures.rocksXLarge.at(3)} } };

		// Large
		std::array<Model, NUM_LARGE_ASTEROID_MODELS>
			asteroidsLarge{
			Model{ "asteroidlarge1", { textures.asteroidsLarge.at(0) } },
				Model{ "asteroidlarge2", {textures.asteroidsLarge.at(1)} } };
		std::array<Model, NUM_LARGE_ASTEROID_MODELS>
			rocksLarge{
			Model{ "asteroidlarge1", { textures.rocksLarge.at(0) } },
				Model{ "asteroidlarge2", {textures.rocksLarge.at(1)} } };

		// Medium
		std::array<Model, NUM_MEDIUM_ASTEROID_MODELS>
			asteroidsMedium{
			Model{ "asteroidmedium1", { textures.asteroidsMedium.at(0) } },
				Model{ "asteroidmedium2", {textures.asteroidsMedium.at(1)} } };
		std::array<Model, NUM_MEDIUM_ASTEROID_MODELS>
			rocksMedium{
			Model{ "asteroidmedium1", { textures.rocksMedium.at(0) } },
				Model{ "asteroidmedium2", {textures.rocksMedium.at(1)} } };

		// Small
		std::array<Model, NUM_SMALL_ASTEROID_MODELS>
			asteroidsSmall{
			Model{ "asteroidsmall1", { textures.asteroidsSmall.at(0) } },
				Model{ "asteroidsmall2", {textures.asteroidsSmall.at(1)} } };
		std::array<Model, NUM_SMALL_ASTEROID_MODELS>
			rocksSmall{
			Model{ "asteroidsmall1", { textures.rocksSmall.at(0) } },
				Model{ "asteroidsmall2", { textures.rocksSmall.at(1)} } };

		// Projectiles
		Model bullet{ "fireball1", {textures.bullet} };
		Model missile{ "rocket005",
			d2d::AnimationDef{ {d2d::AnimationFrame{textures.missileFrames.at(0), MISSILE_TIME_PER_FRAME},
								d2d::AnimationFrame{textures.missileFrames.at(1), MISSILE_TIME_PER_FRAME}},
			d2d::AnimationType::LOOP} };
		Model fatMissile{ "rocket006",
			d2d::AnimationDef{ {d2d::AnimationFrame{textures.fatMissileFrames.at(0), MISSILE_TIME_PER_FRAME},
								d2d::AnimationFrame{textures.fatMissileFrames.at(1), MISSILE_TIME_PER_FRAME}},
			d2d::AnimationType::LOOP} };

		//+---------------------------\-------------------------------
		//|		  Projectiles		  |
		//\---------------------------/-------------------------------
		// Bullet
		ProjectileDef bulletDef{ bullet,
			BULLET_MATERIAL,
			{BULLET_HEIGHT * textures.bullet.GetWidthToHeightRatio(), BULLET_HEIGHT},
			BULLET_FIXED_ROTATION,
			BULLET_CONTINUOUS_COLLISION_DETECTION,
			BULLET_FILTER,
			BULLET_DESTRUCTION_DELAY,
			BULLET_DESTRUCTION_DELAY_TIME,
			BULLET_DESTRUCTION_DELAY_ON_CONTACT,
			BULLET_DESTRUCTION_DELAY_ON_CONTACT_TIME,
			BULLET_DESTRUCTION_CHANCE_ON_CONTACT,
			BULLET_DESTRUCTION_CHANCE,
			BULLET_IGNORE_PARENT_COLLISIONS_UNTIL_FIRST_CONTACT,
			BULLET_ACCELERATION,
			BULLET_ACCELERATION_TIME };

		// Missile
		ProjectileDef missileDef{ missile,
			MISSILE_MATERIAL,
			{MISSILE_HEIGHT * textures.missileFrames[0].GetWidthToHeightRatio(), MISSILE_HEIGHT},
			MISSILE_FIXED_ROTATION,
			MISSILE_CONTINUOUS_COLLISION_DETECTION,
			MISSILE_FILTER,
			MISSILE_DESTRUCTION_DELAY,
			MISSILE_DESTRUCTION_DELAY_TIME,
			MISSILE_DESTRUCTION_DELAY_ON_CONTACT,
			MISSILE_DESTRUCTION_DELAY_ON_CONTACT_TIME,
			MISSILE_DESTRUCTION_CHANCE_ON_CONTACT,
			MISSILE_DESTRUCTION_CHANCE,
			MISSILE_IGNORE_PARENT_COLLISIONS_UNTIL_FIRST_CONTACT,
			MISSILE_ACCELERATION,
			MISSILE_ACCELERATION_TIME };

		// Fat missile
		ProjectileDef fatMissileDef{ fatMissile,
			MISSILE_MATERIAL,
			{MISSILE_HEIGHT * textures.fatMissileFrames[0].GetWidthToHeightRatio(), MISSILE_HEIGHT},
			MISSILE_FIXED_ROTATION,
			MISSILE_CONTINUOUS_COLLISION_DETECTION,
			MISSILE_FILTER,
			MISSILE_DESTRUCTION_DELAY,
			MISSILE_DESTRUCTION_DELAY_TIME,
			MISSILE_DESTRUCTION_DELAY_ON_CONTACT,
			MISSILE_DESTRUCTION_DELAY_ON_CONTACT_TIME,
			MISSILE_DESTRUCTION_CHANCE_ON_CONTACT,
			MISSILE_DESTRUCTION_CHANCE,
			MISSILE_IGNORE_PARENT_COLLISIONS_UNTIL_FIRST_CONTACT,
			MISSILE_ACCELERATION,
			MISSILE_ACCELERATION_TIME };
	};
}
