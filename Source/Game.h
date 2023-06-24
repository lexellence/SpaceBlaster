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
#include "Starfield.h"
#include "GameSettings.h"
#include "GameModels.h"
namespace Space
{
	enum class GameAction
	{
		NEXT_LEVEL,
		RESTART_LEVEL
	};
	struct DelayedGameAction
	{
		GameAction action;
		float delay{};
		float timeElapsed{};
	};

	class Game
		: public DestroyListener,
		  public WrapListener,
		  public ProjectileLauncherListener,
		  public ExitListener
	{
	public:
		Game() = delete;
		Game(Camera* cameraPtr, Starfield* starfieldPtr);
		void Update(float dt, PlayerController &playerController);
		void Draw();
		bool DidPlayerExit() const;
		void StartCurrentLevel();

		// World callbacks
		void EntityWillBeDestroyed(WorldID entityID) override;
		void EntityWrapped(WorldID entityID, const b2Vec2 &translation) override;
		void ProjectileLaunched(const ProjectileDef& projectileDef, WorldID parentID) override;
		void EntityExited(WorldID entityID) override;

	private:
		void ClearLevel(const b2Vec2& newWorldDimensions);
		void SpawnPlayer();
		void SpawnExit();
		unsigned SpawnRandomIcons(unsigned count);
		unsigned SpawnRandomXLargeAsteroids(unsigned count);
		unsigned SpawnRandomLargeAsteroids(unsigned count);
		unsigned SpawnRandomMediumAsteroids(unsigned count);
		unsigned SpawnRandomSmallAsteroids(unsigned count);
		void ValidateWorldDimensions() const;

		void UpdateCamera(float dt, const PlayerController &playerController);
		void UpdateDelayedActions(float dt);
		void SetPlayer(WorldID entityID);
		bool IsPlayer(WorldID entityID) const;
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

		void DrawHUD();

	private:
		World m_world;
		Camera *const m_cameraPtr;
		Starfield *const m_starfieldPtr;
		bool m_firstUpdate{ true };
		GameModels m_models;
		bool m_cameraFollowingEntity{ false };
		WorldID m_cameraFollowEntityID{};
		b2Vec2 m_spawnAsteroidStartingDirection{};

		struct
		{
			bool isSet{};
			WorldID id{};
			float credits{};
			unsigned currentLevel{ 1 };
			bool exited{};
			std::vector<std::string> upgradeList;
		} m_player;
		
		std::list<DelayedGameAction> m_delayedGameActions;

		// Fonts
		d2d::FontReference m_hudFont{"Fonts/OrbitronLight.otf"};
		float m_hudFontSize{0.05f};

		// HUD Fuel
		const b2Vec2 m_fuelPosition{0.10f, 0.10f};
		const d2d::AlignmentAnchor m_fuelAlignment{ d2d::AlignmentAnchorX::LEFT, d2d::AlignmentAnchorY::BOTTOM };
		const d2d::TextStyle m_fuelTextStyle{ &m_hudFont, 
			{1.0f, 0.2f, 0.2f, 1.0f}, m_hudFontSize };

		// HUD Icons Collected
		const b2Vec2 m_iconsPosition{0.10f, 0.90f};
		const d2d::AlignmentAnchor m_iconsAlignment{ d2d::AlignmentAnchorX::LEFT, d2d::AlignmentAnchorY::TOP };
		const d2d::TextStyle m_iconsTextStyle{ &m_hudFont,
			{0.2f, 0.2f, 1.0f, 1.0f}, m_hudFontSize };

		// HUD Credits
		const b2Vec2 m_creditsPosition{0.90f, 0.90f};
		const d2d::AlignmentAnchor m_creditsAlignment{ d2d::AlignmentAnchorX::RIGHT, d2d::AlignmentAnchorY::TOP };
		const d2d::TextStyle m_creditsTextStyle{ &m_hudFont,
			{0.2f, 1.0f, 0.2f, 1.0f}, m_hudFontSize };

		// HUD Level
		const b2Vec2 m_levelPosition{0.90f, 0.10f};
		const d2d::AlignmentAnchor m_levelAlignment{ d2d::AlignmentAnchorX::RIGHT, d2d::AlignmentAnchorY::BOTTOM };
		const d2d::TextStyle m_levelTextStyle{ &m_hudFont,
			{1.0f, 1.0f, 0.2f, 1.0f}, m_hudFontSize };

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
