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
#include "GameModelData.h"
#include "Camera.h"
#include "GameDef.h"
#include "Starfield.h"
namespace Space
{
	const int DEFAULT_DRAW_LAYER = 0;
	const float MIN_WORLD_TO_CAMERA_RATIO = 1.5f;
	const float BLASTER_ROTATION_SPEED = 6.0f;
	const float SCOUT_ROTATION_SPEED = 6.0f;

	const float BLASTER_THRUSTER_ACCELERATION = 8.0f;
	const float SCOUT_THRUSTER_ACCELERATION = 20.0f;
	const float BLASTER_THRUSTER_FUEL_PER_SECOND = 0.02f * BLASTER_THRUSTER_ACCELERATION;
	const float SCOUT_THRUSTER_FUEL_PER_SECOND = 0.02f * SCOUT_THRUSTER_ACCELERATION;

	const float BLASTER_MAX_FUEL = 100.0f;
	const float SCOUT_MAX_FUEL = 25.0f;

	const float BLASTER_BOOST_FACTOR = 8.0f;
	const float SCOUT_BOOST_FACTOR = 5.0f;
	const float BOOST_SECONDS = 0.2f;
	const float BOOST_COOLDOWN_SECONDS = 1.0f;

	const float BLASTER_THRUSTER_OFFSET_X = -0.44f;
	const float SCOUT_THRUSTER_OFFSET_X = -0.9f;
	const float BLASTER_THRUSTER_INNER_SPREAD_Y = 0.33f;
	const float BLASTER_THRUSTER_OUTER_SPREAD_Y = 0.437f;
	const float SCOUT_THRUSTER_SPREAD_Y = 0.1f;

	const b2Vec2 BLASTER_THRUSTER_RELATIVE_SIZE{ 0.25f, 0.075f };
	const b2Vec2 BLASTER_BOOST_THRUSTER_RELATIVE_SIZE{ BLASTER_THRUSTER_RELATIVE_SIZE };
	const b2Vec2 SCOUT_THRUSTER_RELATIVE_SIZE{ 1.5f, 0.15f};

	const float BLASTER_BRAKE_DECELERATION = 40.0f;
	const float SCOUT_BRAKE_DECELERATION = 40.0f;

	const float BLASTER_PROJECTILE_OFFSET_X = 0.1f;
	const float SCOUT_PROJECTILE_OFFSET_X = 0.1f;
	const float BLASTER_PROJECTILE_INNER_SPREAD_Y = 0.327f;
	const float BLASTER_PROJECTILE_OUTER_SPREAD_Y = 0.437f;
	const float SCOUT_PROJECTILE_SPREAD_Y = 0.437f;

	const float BLASTER_CANON_IMPULSE = 5.0f;
	const float SCOUT_CANON_IMPULSE = 4.0f;
	const float BLASTER_CANON_INTERVAL = 0.2f;
	const float SCOUT_CANON_INTERVAL = 0.4f;
	const float BLASTER_MISSILE_LAUNCHER_IMPULSE = 1.0f;
	const float BLASTER_MISSILE_LAUNCHER_INTERVAL = 0.3f;

	const bool BULLET_FIXED_ROTATION = false;
	const bool MISSILE_FIXED_ROTATION = false;
	const bool BULLET_CONTINUOUS_COLLISION_DETECTION = true;
	const bool MISSILE_CONTINUOUS_COLLISION_DETECTION = true;
	const bool BULLET_DESTRUCTION_DELAY = true;
	const bool MISSILE_DESTRUCTION_DELAY = true;
	const float BULLET_DESTRUCTION_DELAY_TIME = 10.0f;
	const float MISSILE_DESTRUCTION_DELAY_TIME = 10.0f;
	const bool BULLET_DESTRUCTION_DELAY_ON_CONTACT = true;
	const bool MISSILE_DESTRUCTION_DELAY_ON_CONTACT = true;
	const float BULLET_DESTRUCTION_DELAY_ON_CONTACT_TIME = 2.0f;
	const float MISSILE_DESTRUCTION_DELAY_ON_CONTACT_TIME = 2.0f;
	const bool BULLET_DESTRUCTION_CHANCE_ON_CONTACT = true;
	const bool MISSILE_DESTRUCTION_CHANCE_ON_CONTACT = true;
	const float BULLET_DESTRUCTION_CHANCE = 0.95f;
	const float MISSILE_DESTRUCTION_CHANCE = 1.0f;
	const bool BULLET_IGNORE_PARENT_COLLISIONS_UNTIL_FIRST_CONTACT = true;
	const bool MISSILE_IGNORE_PARENT_COLLISIONS_UNTIL_FIRST_CONTACT = true;
	const float BULLET_ACCELERATION = 0.0f;
	const float MISSILE_ACCELERATION = 20.0f;
	const float BULLET_ACCELERATION_TIME = 0.0f;
	const float MISSILE_ACCELERATION_TIME = 5.0f;

	const float BLASTER_HP = 200.0f;
	const float SCOUT_HP = 30.0f;
	const float XLARGE_ASTEROID_HP = 115.0f;
	const float LARGE_ASTEROID_HP = 65.0f;
	const float MEDIUM_ASTEROID_HP = 40.0f;
	const float SMALL_ASTEROID_HP = 25.0f;

	const d2d::Material SHIP_MATERIAL{ /*density*/ 1.0f, /*friction*/ 0.5f, /*restitution*/ 0.5f };
	const d2d::Material BULLET_MATERIAL{ /*density*/ 2.0f, /*friction*/ 0.9f, /*restitution*/ 0.2f };
	const d2d::Material MISSILE_MATERIAL{ /*density*/ 0.8f, /*friction*/ 0.7f, /*restitution*/ 0.7f };
	const d2d::Material ASTEROID_MATERIAL{ /*density*/ 1.5f, /*friction*/ 0.6f, /*restitution*/ 0.5f };
	const d2d::Material BUMPER_MATERIAL{ /*density*/ 20.0f, /*friction*/ 0.99f, /*restitution*/ 0.1f };
	const d2d::Material FUEL_MATERIAL{ /*density*/ 0.25f, /*friction*/ 0.6f, /*restitution*/ 0.5f };
	const d2d::Material ICON_MATERIAL{ /*density*/ 0.4f, /*friction*/ 0.6f, /*restitution*/ 0.5f };

	const d2d::Filter DEFAULT_FILTER{ d2d::FILTER_DEFAULT_CATEGORY_BITS, d2d::FILTER_DEFAULT_MASK_BITS, d2d::FILTER_DEFAULT_GROUP_INDEX };
	const d2d::Filter SHIP_FILTER{ DEFAULT_FILTER };
	const d2d::Filter BULLET_FILTER{ d2d::FILTER_DEFAULT_CATEGORY_BITS, d2d::FILTER_DEFAULT_MASK_BITS, -10 };
	const d2d::Filter MISSILE_FILTER{ d2d::FILTER_DEFAULT_CATEGORY_BITS, d2d::FILTER_DEFAULT_MASK_BITS, -10 };
	const d2d::Filter ASTEROID_FILTER{ DEFAULT_FILTER };
	const d2d::Filter BUMPER_FILTER{ DEFAULT_FILTER };
	const d2d::Filter FUEL_FILTER{ DEFAULT_FILTER };
	const d2d::Filter ICON_FILTER{ DEFAULT_FILTER };

	const float BLASTER_HEIGHT = 4.0f;
	const float SCOUT_HEIGHT = BLASTER_HEIGHT * SCOUT_TO_BLASTER_HEIGHT_RATIO * 1.5f;
	const float BULLET_HEIGHT = 0.35f;
	const float SMALL_BULLET_HEIGHT = 0.5f * BULLET_HEIGHT;
	const float MISSILE_HEIGHT = 0.5f;
	const float FAT_MISSILE_HEIGHT = FAT_MISSILE_TO_MISSILE_HEIGHT_RATIO * MISSILE_HEIGHT * 1.2f;
	const float MISSILE_TIME_PER_FRAME = 0.2f;
	const float XLARGE_ASTEROID_HEIGHT = 16.0f;
	const float LARGE_ASTEROID_HEIGHT = LARGE_TO_XLARGE_ASTEROID_AVERAGE_HEIGHT_RATIO * XLARGE_ASTEROID_HEIGHT;
	const float MEDIUM_ASTEROID_HEIGHT = MEDIUM_TO_XLARGE_ASTEROID_AVERAGE_HEIGHT_RATIO * XLARGE_ASTEROID_HEIGHT;
	const float SMALL_ASTEROID_HEIGHT = SMALL_TO_XLARGE_ASTEROID_AVERAGE_HEIGHT_RATIO * XLARGE_ASTEROID_HEIGHT;
	const float BUMPER_HEIGHT = 3.0f;
	const float SODA_HEIGHT = 3.0f;
	const float MELON_HEIGHT = 2.0f;
	const float APPLE_HEIGHT = 2.0f;
	const float ICON_HEIGHT = 4.0f;

	// Random entity placement
	const float MIN_BOUNDING_RADII_GAP_RELATIVE_TO_HEIGHT_ICONS = 2.0f;
	const float MIN_BOUNDING_RADII_GAP_RELATIVE_TO_HEIGHT = 0.25f;
	const unsigned MAX_ATTEMPTS_PER_ENTITY = 500;

	const float PARTICLE_EXPLOSION_RELATIVE_SIZE = 1.0f;
	const unsigned BLASTER_NUM_PARTICLES = 600;
	const unsigned SCOUT_NUM_PARTICLES = 600;
	const unsigned XLARGE_ASTEROID_NUM_PARTICLES = 80;
	const unsigned LARGE_ASTEROID_NUM_PARTICLES = 40;
	const unsigned MEDIUM_ASTEROID_NUM_PARTICLES = 20;
	const unsigned SMALL_ASTEROID_NUM_PARTICLES = 10;

	const d2d::Range<float> BLASTER_PARTICLE_SPEED_RANGE{ 1.0f, 15.0f };
	const d2d::Range<float> SCOUT_PARTICLE_SPEED_RANGE{ 1.0f, 15.0f };
	const d2d::Range<float> ASTEROID_PARTICLE_SPEED_RANGE{ 1.0f, 15.0f };

	const float DAMAGE_BASED_SPEED_INCREASE_FACTOR = 0.3f;

	const d2d::Range<int> BLASTER_PARTICLE_SIZE_INDEX_RANGE{ 5, 14 };
	const d2d::Range<int> SCOUT_PARTICLE_SIZE_INDEX_RANGE{ 5, 14 };
	const d2d::Range<int> XLARGE_ASTEROID_PARTICLE_SIZE_INDEX_RANGE{ 6, 15 };
	const d2d::Range<int> LARGE_ASTEROID_PARTICLE_SIZE_INDEX_RANGE{ 5, 14 };
	const d2d::Range<int> MEDIUM_ASTEROID_PARTICLE_SIZE_INDEX_RANGE{ 4, 13 };
	const d2d::Range<int> SMALL_ASTEROID_PARTICLE_SIZE_INDEX_RANGE{ 3, 12 };

	const float BLASTER_PARTICLE_LIFETIME = 30.0f;
	const float SCOUT_PARTICLE_LIFETIME = 30.0f;
	const float XLARGE_ASTEROID_PARTICLE_LIFETIME = 10.0f;
	const float LARGE_ASTEROID_PARTICLE_LIFETIME = 8.0f;
	const float MEDIUM_ASTEROID_PARTICLE_LIFETIME = 6.0f;
	const float SMALL_ASTEROID_PARTICLE_LIFETIME = 4.0f;

	const float PARTICLE_EXPLOSION_FADEIN = 0.0f;

	const float BLASTER_PARTICLE_FADEOUT = 5.0f;
	const float SCOUT_PARTICLE_FADEOUT = 5.0f;
	const float XLARGE_ASTEROID_PARTICLE_FADEOUT = 4.0f;
	const float LARGE_ASTEROID_PARTICLE_FADEOUT = 3.0f;
	const float MEDIUM_ASTEROID_PARTICLE_FADEOUT = 2.0f;
	const float SMALL_ASTEROID_PARTICLE_FADEOUT = 1.0f;

	class Game
		: public DestroyListener,
		  public WrapListener,
		  public ProjectileLauncherCallback
	// public MorphListener
	{
	public:
		Game();
		void Init();
		void Update(float dt, PlayerController &playerController);
		void Draw();

		// World callbacks
		void SayGoodbye(WorldID entityID) override;
		void EntityWrapped(WorldID entityID, const b2Vec2 &translation) override;
		WorldID LaunchProjectile(const ProjectileDef &projectileDef, const b2Vec2 &position,
								 float angle, float impulse, const b2Vec2 &parentVelocity, WorldID parentID) override;
		// void MorphedIntoEntity(WorldID replacedEntityID, WorldID newEntityID) override;

	private:
		void InitLevel();
		void ClearLevel();
		void SpawnPlayer();
		void SpawnExit();
		unsigned SpawnRandomIcons(unsigned count);
		unsigned SpawnRandomXLargeAsteroids(unsigned count);
		unsigned SpawnRandomLargeAsteroids(unsigned count);
		unsigned SpawnRandomMediumAsteroids(unsigned count);
		unsigned SpawnRandomSmallAsteroids(unsigned count);
		void VerifyWorldDimensions() const;

		void StartDelayedLevelChange(float delay);
		void UpdateCamera(float dt, const PlayerController &playerController);
		void UpdateObjectives();
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

		Camera m_camera;
		bool m_cameraFollowingEntity{ false };
		WorldID m_cameraFollowEntityID;

		bool m_playerSet{false};
		WorldID m_playerID;

		unsigned m_currentLevel{ 0 };
		bool m_delayedLevelChange{ false };
		float m_levelChangeDelayTime;
		float m_levelChangeDelayTimeAccumulator;
//		std::vector<Objective> m_objectives;

		// Fonts
		d2d::FontReference m_hudFont{"Fonts/OrbitronLight.otf"};
		float m_hudFontSize{0.05f};

		// HUD Objectives
//		const b2Vec2 m_objectivesPosition{0.90f, 0.90f};
//		const d2d::Alignment m_objectivesAlignment{d2d::Alignment::RIGHT_TOP};
//		const d2d::TextStyle m_objectivesTextStyle{
//			m_hudFont,
//			{0.5f, 0.3f, 0.8f, 1.0f},
//			m_hudFontSize};

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
		//|		    Textures		  |
		//\---------------------------/-------------------------------
		// Ships
		d2d::TextureAtlas m_bigShipTextureAtlas{ "Textures/bigships.png"s, "Textures/bigships.xml"s};
		d2d::TextureFromAtlas m_blasterTexture{ m_bigShipTextureAtlas, "ship001"s};
		d2d::TextureFromAtlas m_scoutTexture{ m_bigShipTextureAtlas, "ship002"s};

		// Effects
		d2d::TextureAtlas m_effectTextureAtlas{ "Textures/effects.png"s, "Textures/effects.xml"s};
		d2d::TextureFromAtlas m_thrusterTexture{ m_effectTextureAtlas, "thruster1"s};

		// Items
		d2d::TextureAtlas m_itemTextureAtlas{ "Textures/items.png"s, "Textures/items.xml"s};
		d2d::TextureFromAtlas m_appleTexture{ m_itemTextureAtlas, "apple"s};
		d2d::TextureFromAtlas m_bumperTexture{ m_itemTextureAtlas, "repulser1"s};
		d2d::TextureFromAtlas m_sodaTexture{ m_itemTextureAtlas, "sodacan"s};
		d2d::TextureFromAtlas m_melonTexture{ m_itemTextureAtlas, "watermelon"s};

		// Icons
		d2d::TextureAtlas m_iconTextureAtlas{ "Textures/icons.png"s, "Textures/icons.xml"s };
		std::array<d2d::TextureFromAtlas, NUM_ICON_MODELS> m_iconTextures{{
			{ m_iconTextureAtlas, "Solitaire"s },
			{ m_iconTextureAtlas, "RegistryDocument"s },
			{ m_iconTextureAtlas, "OutlookExpress"s },
			{ m_iconTextureAtlas, "NetworkSetup"s },
			{ m_iconTextureAtlas, "MyDocuments"s },
			{ m_iconTextureAtlas, "MyComputer"s },
			{ m_iconTextureAtlas, "Mouse"s },
			{ m_iconTextureAtlas, "MediaCenterFile"s },
			{ m_iconTextureAtlas, "GenericVideo"s },
			{ m_iconTextureAtlas, "GenericTextDocument"s },
			{ m_iconTextureAtlas, "GenericMedia"s },
			{ m_iconTextureAtlas, "GenericDocument"s },
			{ m_iconTextureAtlas, "GenericAudio"s },
			{ m_iconTextureAtlas, "GameController"s },
			{ m_iconTextureAtlas, "Font"s },
			{ m_iconTextureAtlas, "FolderClosed"s },
			{ m_iconTextureAtlas, "FloppyDisk"s },
			{ m_iconTextureAtlas, "EntireNetwork"s },
			{ m_iconTextureAtlas, "DVD-RW"s },
			{ m_iconTextureAtlas, "DLL"s },
			{ m_iconTextureAtlas, "DiskDefragmenter"s },
			{ m_iconTextureAtlas, "CommandPrompt"s },
			{ m_iconTextureAtlas, "ChangeOrRemovePrograms"s },
			{ m_iconTextureAtlas, "Calculator"s },
			{ m_iconTextureAtlas, "BluetoothDevices"s },
			{ m_iconTextureAtlas, "BAT"s },
			{ m_iconTextureAtlas, "IE6"s },
			{ m_iconTextureAtlas, "GIF"s }
		}};

		// Asteroids
		d2d::TextureAtlas m_asteroidTextureAtlas{ "Textures/asteroids.png"s, "Textures/asteroids.xml"s };
		std::array<d2d::TextureFromAtlas, NUM_XLARGE_ASTEROID_MODELS> m_asteroidXLargeTextures{{
			{ m_asteroidTextureAtlas, "asteroidxlarge1"s },
			{ m_asteroidTextureAtlas, "asteroidxlarge2"s },
			{ m_asteroidTextureAtlas, "asteroidxlarge3"s },
			{ m_asteroidTextureAtlas, "asteroidxlarge4"s } }};
		std::array<d2d::TextureFromAtlas, NUM_XLARGE_ASTEROID_MODELS> m_rockXLargeTextures{{
				{ m_asteroidTextureAtlas, "rockxlarge1"s },
				{ m_asteroidTextureAtlas, "rockxlarge2"s },
				{ m_asteroidTextureAtlas, "rockxlarge3"s },
				{ m_asteroidTextureAtlas, "rockxlarge4"s } }};
		std::array<d2d::TextureFromAtlas, NUM_LARGE_ASTEROID_MODELS> m_asteroidLargeTextures{{
				{ m_asteroidTextureAtlas, "asteroidlarge1"s },
				{ m_asteroidTextureAtlas, "asteroidlarge2"s } }};
		std::array<d2d::TextureFromAtlas, NUM_LARGE_ASTEROID_MODELS> m_rockLargeTextures{{
				{ m_asteroidTextureAtlas, "rocklarge1"s },
				{ m_asteroidTextureAtlas, "rocklarge2"s } }};
		std::array<d2d::TextureFromAtlas, NUM_MEDIUM_ASTEROID_MODELS> m_asteroidMediumTextures{{
				{ m_asteroidTextureAtlas, "asteroidmedium1"s },
				{ m_asteroidTextureAtlas, "asteroidmedium2"s } }};
		std::array<d2d::TextureFromAtlas, NUM_MEDIUM_ASTEROID_MODELS> m_rockMediumTextures{{
				{ m_asteroidTextureAtlas, "rockmedium1"s },
				{ m_asteroidTextureAtlas, "rockmedium2"s } }};
		std::array<d2d::TextureFromAtlas, NUM_SMALL_ASTEROID_MODELS> m_asteroidSmallTextures{{
				{ m_asteroidTextureAtlas, "asteroidsmall1"s },
				{ m_asteroidTextureAtlas, "asteroidsmall2"s } }};
		std::array<d2d::TextureFromAtlas, NUM_SMALL_ASTEROID_MODELS> m_rockSmallTextures{{
				{ m_asteroidTextureAtlas, "rocksmall1"s },
				{ m_asteroidTextureAtlas, "rocksmall2"s } }};

		// Projectiles
		d2d::TextureAtlas m_projectileTextureAtlas{ "Textures/projectiles.png"s, "Textures/projectiles.xml"s };
		d2d::TextureFromAtlas m_bulletTexture{ m_projectileTextureAtlas, "fireball1"s};
		std::array<d2d::TextureFromAtlas, 2> m_missileTextures{{
				{ m_projectileTextureAtlas, "rocket005a"s },
				{ m_projectileTextureAtlas, "rocket005b"s } }};
		std::array<d2d::TextureFromAtlas, 2> m_fatMissileTextures{{
				{ m_projectileTextureAtlas, "rocket006a"s },
				{ m_projectileTextureAtlas, "rocket006b"s } }};

		//+---------------------------\-------------------------------
		//|			 Models 		  |
		//\---------------------------/-------------------------------
		Model m_blasterModel{"ship001", {m_blasterTexture}};
		Model m_scoutModel{"ship002", {m_scoutTexture}};
		Model m_blasterThrusterModel{"thruster1", {{m_thrusterTexture, 0.0f, d2d::WHITE_OPAQUE, BLASTER_THRUSTER_RELATIVE_SIZE}}};
		Model m_scoutThrusterModel{"thruster1", {{m_thrusterTexture, 0.0f, d2d::WHITE_OPAQUE, SCOUT_THRUSTER_RELATIVE_SIZE}}};
		Model m_bumperModel{"repulser1", {m_bumperTexture}};
		Model m_sodaModel{"sodacan", {m_sodaTexture}};
		Model m_melonModel{"watermelon", {m_melonTexture}};
		Model m_appleModel{"apple", {m_appleTexture}};

		// XLarge
		std::array<Model, NUM_XLARGE_ASTEROID_MODELS>
			m_asteroidXLargeModels{
				Model{"asteroidxlarge1", {m_asteroidXLargeTextures.at(0)}},
				Model{"asteroidxlarge2", {m_asteroidXLargeTextures.at(1)}},
				Model{"asteroidxlarge3", {m_asteroidXLargeTextures.at(2)}},
				Model{"asteroidxlarge4", {m_asteroidXLargeTextures.at(3)}}};
		std::array<Model, NUM_XLARGE_ASTEROID_MODELS>
			m_rockXLargeModels{
				Model{"asteroidxlarge1", {m_rockXLargeTextures.at(0)}},
				Model{"asteroidxlarge2", {m_rockXLargeTextures.at(1)}},
				Model{"asteroidxlarge3", {m_rockXLargeTextures.at(2)}},
				Model{"asteroidxlarge4", {m_rockXLargeTextures.at(3)}}};
		// Large
		std::array<Model, NUM_LARGE_ASTEROID_MODELS>
			m_asteroidLargeModels{Model{"asteroidlarge1", {m_asteroidLargeTextures.at(0)}},
								  Model{"asteroidlarge2", {m_asteroidLargeTextures.at(1)}}};
		std::array<Model, NUM_LARGE_ASTEROID_MODELS>
			m_rockLargeModels{Model{"asteroidlarge1", {m_rockLargeTextures.at(0)}},
							  Model{"asteroidlarge2", {m_rockLargeTextures.at(1)}}};
		// Medium
		std::array<Model, NUM_MEDIUM_ASTEROID_MODELS>
			m_asteroidMediumModels{Model{"asteroidmedium1", {m_asteroidMediumTextures.at(0)}},
								   Model{"asteroidmedium2", {m_asteroidMediumTextures.at(1)}}};
		std::array<Model, NUM_MEDIUM_ASTEROID_MODELS>
			m_rockMediumModels{Model{"asteroidmedium1", {m_rockMediumTextures.at(0)}},
							   Model{"asteroidmedium2", {m_rockMediumTextures.at(1)}}};
		// Small
		std::array<Model, NUM_SMALL_ASTEROID_MODELS>
			m_asteroidSmallModels{Model{"asteroidsmall1", {m_asteroidSmallTextures.at(0)}},
								  Model{"asteroidsmall2", {m_asteroidSmallTextures.at(1)}}};
		std::array<Model, NUM_SMALL_ASTEROID_MODELS>
			m_rockSmallModels{Model{"asteroidsmall1", {m_rockSmallTextures.at(0)}},
							  Model{"asteroidsmall2", {m_rockSmallTextures.at(1)}}};
		// Projectiles
		Model m_bulletModel{"fireball1", {m_bulletTexture}};
		Model m_missileModel{"rocket005",
			{{d2d::AnimationFrame{m_missileTextures.at(0), MISSILE_TIME_PER_FRAME},
			  d2d::AnimationFrame{m_missileTextures.at(1), MISSILE_TIME_PER_FRAME}},
			d2d::AnimationType::LOOP, 0, true}};
		Model m_fatMissileModel{"rocket006",
			{{d2d::AnimationFrame{m_fatMissileTextures.at(0), MISSILE_TIME_PER_FRAME},
			  d2d::AnimationFrame{m_fatMissileTextures.at(1), MISSILE_TIME_PER_FRAME}},
			d2d::AnimationType::LOOP, 0, true}};

		//+---------------------------\-------------------------------
		//|		  Projectiles		  |
		//\---------------------------/-------------------------------
		// Bullet
		ProjectileDef m_bulletDef{m_bulletModel,
								  BULLET_MATERIAL,
								  {BULLET_HEIGHT * m_bulletTexture.GetWidthToHeightRatio(), BULLET_HEIGHT},
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
								  BULLET_ACCELERATION_TIME};
		// Missile
		ProjectileDef m_missileDef{m_missileModel,
								   MISSILE_MATERIAL,
								   {MISSILE_HEIGHT * m_missileTextures[0].GetWidthToHeightRatio(), MISSILE_HEIGHT},
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
								   MISSILE_ACCELERATION_TIME};
		// Fat missile
		ProjectileDef m_fatMissileDef{m_fatMissileModel,
									  MISSILE_MATERIAL,
									  {MISSILE_HEIGHT * m_fatMissileTextures[0].GetWidthToHeightRatio(), MISSILE_HEIGHT},
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
									  MISSILE_ACCELERATION_TIME};
	};
}
