/**************************************************************************************\
** File: Game.h
** Project:
** Author: David Leksen
** Date:
**
** Header file for the Game class
**
\**************************************************************************************/
#pragma once
#include "World.h"
#include "Camera.h"
#include "GameDef.h"
#include "Starfield.h"
#include "GameConstants.h"
#include "GameModels.h"
namespace Space
{
	const int DEFAULT_DRAW_LAYER = 0;
	const float MIN_WORLD_TO_CAMERA_RATIO = 1.5f;

	class Game
		: public DestroyListener,
		  public WrapListener,
		  public ProjectileLauncherCallback
	{
	public:
		Game();
		void Init();
		void Update(float dt, PlayerController &playerController);
		void Draw();

		// World callbacks
		void EntityWillBeDestroyed(WorldID entityID) override;
		void EntityWrapped(WorldID entityID, const b2Vec2 &translation) override;
		WorldID LaunchProjectile(const ProjectileDef &projectileDef, const b2Vec2 &position,
								 float angle, float impulse, const b2Vec2 &parentVelocity, WorldID parentID) override;

	private:
		void StartCurrentLevel();
		void ClearLevel(const b2Vec2& newWorldDimensions);
		void SpawnPlayer();
		void SpawnExit();
		unsigned SpawnRandomIcons(unsigned count);
		unsigned SpawnRandomXLargeAsteroids(unsigned count);
		unsigned SpawnRandomLargeAsteroids(unsigned count);
		unsigned SpawnRandomMediumAsteroids(unsigned count);
		unsigned SpawnRandomSmallAsteroids(unsigned count);
		void ValidateWorldDimensions() const;

		void StartDelayedLevelChange(float delay);
		void UpdateCamera(float dt, const PlayerController &playerController);
		void SetPlayer(WorldID entityID);
		void FollowEntity(WorldID entityID);

		WorldID CreateBasicObject(World &world, const b2Vec2 &size, int drawLayer,
								  const Model &model, const d2d::Material &material, const d2d::Filter &filter,
								  b2BodyType physicsType, const InstanceDef &def);
		void CreatePlayer(World &world, const InstanceDef &def);
		WorldID CreateScout(World &world, const InstanceDef &def);
		WorldID CreateBlaster(World &world, const InstanceDef &def);
		WorldID CreateXLargeAsteroid(World &world, unsigned modelIndex, bool isRock, const InstanceDef &def);
		WorldID CreateLargeAsteroid(World &world, unsigned modelIndex, bool isRock, const InstanceDef &def);
		WorldID CreateMediumAsteroid(World &world, unsigned modelIndex, bool isRock, const InstanceDef &def);
		WorldID CreateSmallAsteroid(World &world, unsigned modelIndex, bool isRock, const InstanceDef &def);
		WorldID CreateBumper(World &world, const InstanceDef &def);
		WorldID CreateSoda(World &world, const InstanceDef &def);
		WorldID CreateMelon(World &world, const InstanceDef &def);
		WorldID CreateApple(World &world, const InstanceDef &def);
		WorldID CreateIcon(World &world, unsigned modelIndex, const InstanceDef &def);
		WorldID CreateExit(World &world, const InstanceDef &def);
		WorldID CreateExitSensor(World &world, const InstanceDef &def);

	private:
		GameDef m_settings;
		World m_world;
		Starfield m_starfield;
		GameModels m_models;

		Camera m_camera;
		bool m_cameraFollowingEntity{ false };
		WorldID m_cameraFollowEntityID;

		struct
		{
			bool isSet{ false };
			WorldID id;
		} m_player;
		
		unsigned m_currentLevel{ 0 };
		bool m_delayedLevelChange{ false };
		float m_levelChangeDelayTime;
		float m_levelChangeDelayTimeAccumulator;

		// Fonts
		d2d::FontReference m_hudFont{"Fonts/OrbitronLight.otf"};
		float m_hudFontSize{0.05f};

		// HUD Fuel
		const b2Vec2 m_fuelPosition{0.10f, 0.10f};
		const d2d::Alignment m_fuelAlignment{d2d::Alignment::LEFT_BOTTOM};
		const d2d::TextStyle m_fuelTextStyle{
			m_hudFont,
			{1.0f, 0.2f, 0.2f, 1.0f},
			m_hudFontSize};

		// HUD Icons Collected
		const b2Vec2 m_iconsPosition{0.10f, 0.90f};
		const d2d::Alignment m_iconsAlignment{d2d::Alignment::LEFT_TOP};
		const d2d::TextStyle m_iconsTextStyle{
			m_hudFont,
			{0.2f, 0.2f, 1.0f, 1.0f},
			m_hudFontSize};

		//+---------------------------\-------------------------------
		//|		  Projectiles		  |
		//\---------------------------/-------------------------------
		// Bullet
		ProjectileDef m_bulletDef{ m_models.bullet,
			BULLET_MATERIAL,
			{BULLET_HEIGHT * m_models.textures.bullet.GetWidthToHeightRatio(), BULLET_HEIGHT},
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
		ProjectileDef m_missileDef{ m_models.missile,
			MISSILE_MATERIAL,
			{MISSILE_HEIGHT * m_models.textures.missileFrames[0].GetWidthToHeightRatio(), MISSILE_HEIGHT},
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
		ProjectileDef m_fatMissileDef{ m_models.fatMissile,
			MISSILE_MATERIAL,
			{MISSILE_HEIGHT * m_models.textures.fatMissileFrames[0].GetWidthToHeightRatio(), MISSILE_HEIGHT},
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
