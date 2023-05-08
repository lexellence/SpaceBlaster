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
#include "Exceptions.h"
#include "GameDef.h"
#include "Starfield.h"
namespace Space
{
	constexpr float MIN_WORLD_TO_CAMERA_RATIO{ 1.5f };
	constexpr int SHIP_DRAW_LAYER		{ 0 };
	constexpr int ASTEROID_DRAW_LAYER	{ 0 };
	constexpr int BUMPER_DRAW_LAYER		{ 0 };

	constexpr float BLASTER_ROTATION_SPEED	{ 6.0f };
	constexpr float SCOUT_ROTATION_SPEED	{ 6.0f };

	const float BLASTER_THRUSTER_ACCELERATION	{ 8.0f };
	const float SCOUT_THRUSTER_ACCELERATION		{ 20.0f };
	const float BLASTER_THRUSTER_FUEL_PER_SECOND{ 0.02f * BLASTER_THRUSTER_ACCELERATION };
	const float SCOUT_THRUSTER_FUEL_PER_SECOND	{ 0.02f * SCOUT_THRUSTER_ACCELERATION };

	constexpr float BLASTER_MAX_FUEL{ 100.0f };
	constexpr float SCOUT_MAX_FUEL	{ 25.0f };

	constexpr float BLASTER_BOOST_FACTOR{ 8.0f };
	constexpr float SCOUT_BOOST_FACTOR	{ 5.0f };
	constexpr float BOOST_SECONDS		{ 0.2f };
	constexpr float BOOST_COOLDOWN_SECONDS		{ 1.0f };

	constexpr float BLASTER_THRUSTER_OFFSET_X		{ -0.44f };
	constexpr float SCOUT_THRUSTER_OFFSET_X			{ -0.9f };
	constexpr float BLASTER_THRUSTER_INNER_SPREAD_Y { 0.33f };
	constexpr float BLASTER_THRUSTER_OUTER_SPREAD_Y { 0.437f };
	constexpr float SCOUT_THRUSTER_SPREAD_Y			{ 0.1f };

	const b2Vec2 BLASTER_THRUSTER_RELATIVE_SIZE{ 0.25f, 0.075f };
	const b2Vec2 BLASTER_BOOST_THRUSTER_RELATIVE_SIZE{ BLASTER_THRUSTER_RELATIVE_SIZE };
	const b2Vec2 SCOUT_THRUSTER_RELATIVE_SIZE{ 1.5f, 0.15f };

	constexpr float BLASTER_BRAKE_DECELERATION{ 40.0f };
	constexpr float SCOUT_BRAKE_DECELERATION{ 40.0f };

	constexpr float BLASTER_PROJECTILE_OFFSET_X		  { 0.1f };
	constexpr float SCOUT_PROJECTILE_OFFSET_X		  { 0.1f };
	constexpr float BLASTER_PROJECTILE_INNER_SPREAD_Y { 0.327f };
	constexpr float BLASTER_PROJECTILE_OUTER_SPREAD_Y { 0.437f };
	constexpr float SCOUT_PROJECTILE_SPREAD_Y { 0.437f };

	constexpr float BLASTER_CANON_IMPULSE{ 5.0f };
	constexpr float SCOUT_CANON_IMPULSE{ 4.0f };
	constexpr float BLASTER_CANON_INTERVAL{ 0.2f };
	constexpr float SCOUT_CANON_INTERVAL{ 0.4f };
	constexpr float BLASTER_MISSILE_LAUNCHER_IMPULSE{ 1.0f };
	constexpr float BLASTER_MISSILE_LAUNCHER_INTERVAL{ 0.3f };

	constexpr bool BULLET_FIXED_ROTATION{ false };
	constexpr bool MISSILE_FIXED_ROTATION{ false };
	constexpr bool BULLET_CONTINUOUS_COLLISION_DETECTION{ true };
	constexpr bool MISSILE_CONTINUOUS_COLLISION_DETECTION{ true };
	constexpr bool BULLET_DESTRUCTION_DELAY{ true };
	constexpr bool MISSILE_DESTRUCTION_DELAY{ true };
	constexpr float BULLET_DESTRUCTION_DELAY_TIME{ 10.0f };
	constexpr float MISSILE_DESTRUCTION_DELAY_TIME{ 10.0f };
	constexpr bool BULLET_DESTRUCTION_DELAY_ON_CONTACT{ true };
	constexpr bool MISSILE_DESTRUCTION_DELAY_ON_CONTACT{ true };
	constexpr float BULLET_DESTRUCTION_DELAY_ON_CONTACT_TIME{ 2.0f };
	constexpr float MISSILE_DESTRUCTION_DELAY_ON_CONTACT_TIME{ 2.0f };
	constexpr bool BULLET_DESTRUCTION_CHANCE_ON_CONTACT{ true };
	constexpr bool MISSILE_DESTRUCTION_CHANCE_ON_CONTACT{ true };
	constexpr float BULLET_DESTRUCTION_CHANCE{ 0.95f };
	constexpr float MISSILE_DESTRUCTION_CHANCE{ 1.0f };
	constexpr bool BULLET_IGNORE_PARENT_COLLISIONS_UNTIL_FIRST_CONTACT{ true };
	constexpr bool MISSILE_IGNORE_PARENT_COLLISIONS_UNTIL_FIRST_CONTACT{ true };
	constexpr float BULLET_ACCELERATION{ 0.0f };
	constexpr float MISSILE_ACCELERATION{ 20.0f };
	constexpr float BULLET_ACCELERATION_TIME{ 0.0f };
	constexpr float MISSILE_ACCELERATION_TIME{ 5.0f };

	constexpr float BLASTER_HP			{ 100.0f };
	constexpr float SCOUT_HP			{ 30.0f };
	constexpr float XLARGE_ASTEROID_HP	{ 80.0f };
	constexpr float LARGE_ASTEROID_HP	{ 50.0f };
	constexpr float MEDIUM_ASTEROID_HP	{ 30.0f };
	constexpr float SMALL_ASTEROID_HP	{ 20.0f };

	const d2d::Material SHIP_MATERIAL	 { /*density*/ 1.0f, /*friction*/ 0.5f, /*restitution*/ 0.5f };
	const d2d::Material BULLET_MATERIAL  { /*density*/ 2.0f, /*friction*/ 0.9f, /*restitution*/ 0.2f };
	const d2d::Material MISSILE_MATERIAL { /*density*/ 0.8f, /*friction*/ 0.7f, /*restitution*/ 0.7f };
	const d2d::Material ASTEROID_MATERIAL{ /*density*/ 1.5f, /*friction*/ 0.6f, /*restitution*/ 0.5f };
	const d2d::Material BUMPER_MATERIAL  { /*density*/ 1.5f, /*friction*/ 0.6f, /*restitution*/ 0.5f };

	const d2d::Filter SHIP_FILTER		{ d2d::FILTER_DEFAULT_CATEGORY_BITS, d2d::FILTER_DEFAULT_MASK_BITS, d2d::FILTER_DEFAULT_GROUP_INDEX };
	const d2d::Filter BULLET_FILTER		{ d2d::FILTER_DEFAULT_CATEGORY_BITS, d2d::FILTER_DEFAULT_MASK_BITS, -10 };
	const d2d::Filter MISSILE_FILTER	{ d2d::FILTER_DEFAULT_CATEGORY_BITS, d2d::FILTER_DEFAULT_MASK_BITS, -10 };
	const d2d::Filter ASTEROID_FILTER	{ d2d::FILTER_DEFAULT_CATEGORY_BITS, d2d::FILTER_DEFAULT_MASK_BITS, d2d::FILTER_DEFAULT_GROUP_INDEX };
	const d2d::Filter BUMPER_FILTER		{ d2d::FILTER_DEFAULT_CATEGORY_BITS, d2d::FILTER_DEFAULT_MASK_BITS, d2d::FILTER_DEFAULT_GROUP_INDEX };
	
	constexpr float BLASTER_HEIGHT{ 4.0f };
	constexpr float SCOUT_HEIGHT{ BLASTER_HEIGHT * SCOUT_TO_BLASTER_HEIGHT_RATIO * 1.5f };
	constexpr float BULLET_HEIGHT{ 0.35f };
	constexpr float SMALL_BULLET_HEIGHT{ 0.5f * BULLET_HEIGHT };
	constexpr float MISSILE_HEIGHT{ 0.5f };
	constexpr float FAT_MISSILE_HEIGHT{ FAT_MISSILE_TO_MISSILE_HEIGHT_RATIO * MISSILE_HEIGHT * 1.2f };
	constexpr float MISSILE_TIME_PER_FRAME{ 0.2f };
	constexpr float XLARGE_ASTEROID_HEIGHT{ 16.0f };
	const float LARGE_ASTEROID_HEIGHT { LARGE_TO_XLARGE_ASTEROID_AVERAGE_HEIGHT_RATIO * XLARGE_ASTEROID_HEIGHT };
	const float MEDIUM_ASTEROID_HEIGHT{ MEDIUM_TO_XLARGE_ASTEROID_AVERAGE_HEIGHT_RATIO * XLARGE_ASTEROID_HEIGHT };
	const float SMALL_ASTEROID_HEIGHT { SMALL_TO_XLARGE_ASTEROID_AVERAGE_HEIGHT_RATIO * XLARGE_ASTEROID_HEIGHT };
	const float BUMPER_HEIGHT{ 3.0f };
	
	const float PARTICLE_EXPLOSION_RELATIVE_SIZE{ 1.0f };
	const unsigned BLASTER_NUM_PARTICLES{ 600 };
	const unsigned SCOUT_NUM_PARTICLES{ 600 };
	const unsigned XLARGE_ASTEROID_NUM_PARTICLES{ 80 };
	const unsigned LARGE_ASTEROID_NUM_PARTICLES { 40 };
	const unsigned MEDIUM_ASTEROID_NUM_PARTICLES{ 20 };
	const unsigned SMALL_ASTEROID_NUM_PARTICLES { 10 };

	const d2d::Range<float> BLASTER_PARTICLE_SPEED_RANGE{ 1.0f, 15.0f };
	const d2d::Range<float> SCOUT_PARTICLE_SPEED_RANGE{ 1.0f, 15.0f };
	const d2d::Range<float> ASTEROID_PARTICLE_SPEED_RANGE{ 1.0f, 15.0f };

	const float DAMAGE_BASED_SPEED_INCREASE_FACTOR{ 0.3f };

	const d2d::Range<int> BLASTER_PARTICLE_SIZE_INDEX_RANGE		{ 5, 14 };
	const d2d::Range<int> SCOUT_PARTICLE_SIZE_INDEX_RANGE			{ 5, 14 };
	const d2d::Range<int> XLARGE_ASTEROID_PARTICLE_SIZE_INDEX_RANGE{ 6, 15 };
	const d2d::Range<int> LARGE_ASTEROID_PARTICLE_SIZE_INDEX_RANGE { 5, 14 };
	const d2d::Range<int> MEDIUM_ASTEROID_PARTICLE_SIZE_INDEX_RANGE{ 4, 13 };
	const d2d::Range<int> SMALL_ASTEROID_PARTICLE_SIZE_INDEX_RANGE { 3, 12 };

	const float BLASTER_PARTICLE_LIFETIME		 { 30.0f };
	const float SCOUT_PARTICLE_LIFETIME			 { 30.0f };
	const float XLARGE_ASTEROID_PARTICLE_LIFETIME{ 10.0f };
	const float LARGE_ASTEROID_PARTICLE_LIFETIME { 8.0f };
	const float MEDIUM_ASTEROID_PARTICLE_LIFETIME{ 6.0f };
	const float SMALL_ASTEROID_PARTICLE_LIFETIME { 4.0f };

	const float PARTICLE_EXPLOSION_FADEIN		{ 0.0f };

	const float BLASTER_PARTICLE_FADEOUT		{ 5.0f };
	const float SCOUT_PARTICLE_FADEOUT			{ 5.0f };
	const float XLARGE_ASTEROID_PARTICLE_FADEOUT{ 4.0f };
	const float LARGE_ASTEROID_PARTICLE_FADEOUT { 3.0f };
	const float MEDIUM_ASTEROID_PARTICLE_FADEOUT{ 2.0f };
	const float SMALL_ASTEROID_PARTICLE_FADEOUT { 1.0f };

	class Game 
	  : public DestroyListener, 
		public WrapListener, 
		public ProjectileLauncherCallback
		//public MorphListener
	{
	public:
		Game();
		void Init();
		void Update(float dt, PlayerController& playerController);
		void Draw();

		// World callbacks
		void SayGoodbye(WorldID entityID) override;
		void EntityWrapped(WorldID entityID, const b2Vec2& translation) override;
		WorldID LaunchProjectile(const ProjectileDef& projectileDef, const b2Vec2& position,
			float angle, float impulse, const b2Vec2& parentVelocity, WorldID parentID) override;
		//void MorphedIntoEntity(WorldID replacedEntityID, WorldID newEntityID) override;

	private:
		void InitLevel(const std::string& level);
		void StartDelayedLevelChange(const std::string& level, float delay);
		void UpdateCamera(float dt, const PlayerController& playerController);
		void UpdateObjectives();
		void LoadTest0();
		void LoadTest1();
		void LoadTest2();
		void LoadLevel0();
		void SetPlayer(WorldID entityID);
		void FollowEntity(WorldID entityID);
		void CreatePlayer(World& world, const InstanceDef& def);
		WorldID CreateScout(World& world, const InstanceDef& def);
		WorldID CreateBlaster(World& world, const InstanceDef& def);
		WorldID CreateXLargeAsteroid(World& world, unsigned modelIndex, bool isRock, const InstanceDef& def);
		WorldID CreateLargeAsteroid(World& world, unsigned modelIndex, bool isRock, const InstanceDef& def);
		WorldID CreateMediumAsteroid(World& world, unsigned modelIndex, bool isRock, const InstanceDef& def);
		WorldID CreateSmallAsteroid(World& world, unsigned modelIndex, bool isRock, const InstanceDef& def);
		WorldID CreateBumper(World& world, const InstanceDef& def);
		WorldID CreateFuel(World& world, const InstanceDef& def);

	private:
		GameDef m_settings;
		World m_world;
		Starfield m_starfield;

		Camera m_camera;
		bool m_cameraFollowingEntity{ false };	
		WorldID m_cameraFollowEntityID;

		bool m_playerSet{ false };
		WorldID m_playerID;

		bool m_startOver{ false };
		std::string m_currentLevel;
		bool m_delayedLevelChange{ false };
		std::string m_delayedLevel;
		float m_levelChangeDelayTime;
		float m_levelChangeDelayTimeAccumulator;
		std::vector<Objective> m_objectives;

		// Fonts
		d2d::FontReference m_hudFont{ "Fonts/OrbitronLight.otf" };
		float m_hudFontSize{ 0.05f };

		// HUD Objectives
		const b2Vec2 m_objectivesPosition{ 0.90f, 0.90f };
		const d2d::Alignment m_objectivesAlignment{ d2d::Alignment::RIGHT_TOP };
		const d2d::TextStyle m_objectivesTextStyle{
			m_hudFont,
			{ 0.5f, 0.3f, 0.8f, 1.0f },
			m_hudFontSize
		};

		// HUD Fuel
		const b2Vec2 m_fuelPosition{ 0.10f, 0.10f };
		const d2d::Alignment m_fuelAlignment{ d2d::Alignment::LEFT_BOTTOM };
		const d2d::TextStyle m_fuelTextStyle{
			m_hudFont,
			{ 1.0f, 0.2f, 0.2f, 1.0f },
			m_hudFontSize
		};

		//+---------------------------\-------------------------------
		//|		    Textures		  |
		//\---------------------------/-------------------------------
		d2d::TextureReference m_blasterTexture{ "Textures/BigShips/ship001.png" };
		d2d::TextureReference m_scoutTexture{ "Textures/BigShips/ship002.png" };
		d2d::TextureReference m_thrusterTexture{ "Textures/Effects/thruster1.png" };
		d2d::TextureReference m_bumperTexture{ "Textures/Items/repulser1.png" };
		d2d::TextureReference m_sodaTexture{ "Textures/Items/sodacan.png" };
		d2d::TextureReference m_melonTexture{ "Textures/Items/watermelon.png" };
		d2d::TextureReference m_appleTexture{ "Textures/Items/apple.png" };

		// Icons
		std::array<d2d::TextureReference, NUM_ICON_MODELS>
			m_iconTextures{ "Textures/Icons/Solitaire.png"s, "Textures/Icons/RegistryDocument.png"s,
			"Textures/Icons/OutlookExpress.png"s, "Textures/Icons/NetworkSetup.png"s,
			"Textures/Icons/MyDocuments.png"s, "Textures/Icons/MyComputer.png"s,
			"Textures/Icons/Mouse.png"s, "Textures/Icons/MediaCenterFile.png"s,
			"Textures/Icons/GenericVideo.png"s, "Textures/Icons/MediaCenterFile.png"s,
			"Textures/Icons/GenericTextDocument.png"s, "Textures/Icons/GenericMedia.png"s,
			"Textures/Icons/GenericDocument.png"s, "Textures/Icons/GenericAudio.png"s,
			"Textures/Icons/GameController.png"s, "Textures/Icons/Font.png"s,
			"Textures/Icons/FolderClosed.png"s, "Textures/Icons/FloppyDisk.png"s,
			"Textures/Icons/EntireNetwork.png"s, "Textures/Icons/DVD-RW.png"s,
			"Textures/Icons/DLL.png"s, "Textures/Icons/DiskDefragmenter.png"s,
			"Textures/Icons/CommandPrompt.png"s, "Textures/Icons/ChangeOrRemovePrograms.png"s,
			"Textures/Icons/Calculator.png"s, "Textures/Icons/BluetoothDevices.png"s,
			"Textures/Icons/BAT.png"s,
			"Textures/Icons/IE6.png"s, "Textures/Icons/GIF.png"s };
		// XLarge
		std::array<d2d::TextureReference, NUM_XLARGE_ASTEROID_MODELS>
			m_asteroidXLargeTextures{ "Textures/Asteroids/asteroidxlarge1.png"s, "Textures/Asteroids/asteroidxlarge2.png"s,
			"Textures/Asteroids/asteroidxlarge3.png"s, "Textures/Asteroids/asteroidxlarge4.png"s };
		std::array<d2d::TextureReference, NUM_XLARGE_ASTEROID_MODELS>
			m_rockXLargeTextures{ "Textures/Asteroids/rockxlarge1.png"s, "Textures/Asteroids/rockxlarge2.png"s,
			"Textures/Asteroids/rockxlarge3.png"s, "Textures/Asteroids/rockxlarge4.png"s };
		// Large
		std::array<d2d::TextureReference, NUM_LARGE_ASTEROID_MODELS>
			m_asteroidLargeTextures{ "Textures/Asteroids/asteroidlarge1.png"s, "Textures/Asteroids/asteroidlarge2.png"s };
		std::array<d2d::TextureReference, NUM_LARGE_ASTEROID_MODELS>
			m_rockLargeTextures{ "Textures/Asteroids/rocklarge1.png"s, "Textures/Asteroids/rocklarge2.png"s };
		// Medium
		std::array<d2d::TextureReference, NUM_MEDIUM_ASTEROID_MODELS>
			m_asteroidMediumTextures{ "Textures/Asteroids/asteroidmedium1.png"s, "Textures/Asteroids/asteroidmedium2.png"s };
		std::array<d2d::TextureReference, NUM_MEDIUM_ASTEROID_MODELS>
			m_rockMediumTextures{ "Textures/Asteroids/rockmedium1.png"s, "Textures/Asteroids/rockmedium2.png"s };
		// Small
		std::array<d2d::TextureReference, NUM_SMALL_ASTEROID_MODELS>
			m_asteroidSmallTextures{ "Textures/Asteroids/asteroidsmall1.png"s, "Textures/Asteroids/asteroidsmall2.png"s };
		std::array<d2d::TextureReference, NUM_SMALL_ASTEROID_MODELS>
			m_rockSmallTextures{ "Textures/Asteroids/rocksmall1.png"s, "Textures/Asteroids/rocksmall2.png"s };
		// Projectiles
		d2d::TextureReference m_bulletTexture{ "Textures/Projectiles/fireball1.png" };
		std::array<d2d::TextureReference, 2>
			m_missileTextures{ "Textures/Projectiles/rocket005a.png"s, "Textures/Projectiles/rocket005b.png"s };
		std::array<d2d::TextureReference, 2>
			m_fatMissileTextures{ "Textures/Projectiles/rocket006a.png"s, "Textures/Projectiles/rocket006b.png"s };

		//+---------------------------\-------------------------------
		//|			 Models 		  |
		//\---------------------------/-------------------------------
		Model m_blasterModel{ "ship001", { &m_blasterTexture } };
		Model m_scoutModel{ "ship002", { &m_scoutTexture } };
		Model m_blasterThrusterModel{ "thruster1", {{ &m_thrusterTexture, 0.0f, d2d::WHITE_OPAQUE, BLASTER_THRUSTER_RELATIVE_SIZE }} };
		Model m_scoutThrusterModel{ "thruster1", {{ &m_thrusterTexture, 0.0f, d2d::WHITE_OPAQUE, SCOUT_THRUSTER_RELATIVE_SIZE }} };
		Model m_bumperModel{ "repulser1", { &m_bumperTexture } };
		d2d::Color m_boostColor{ 0.8f, 0.1f, 0.8f };

		// XLarge
		std::array<Model, NUM_XLARGE_ASTEROID_MODELS> 
			m_asteroidXLargeModels{
				Model{ "asteroidxlarge1", { &m_asteroidXLargeTextures.at(0)}},
				Model{ "asteroidxlarge2", { &m_asteroidXLargeTextures.at(1) } },
				Model{ "asteroidxlarge3", { &m_asteroidXLargeTextures.at(2) } },
				Model{ "asteroidxlarge4", { &m_asteroidXLargeTextures.at(3) } } };
		std::array<Model, NUM_XLARGE_ASTEROID_MODELS>
			m_rockXLargeModels{
				Model{ "asteroidxlarge1", { &m_rockXLargeTextures.at(0) } },
				Model{ "asteroidxlarge2", { &m_rockXLargeTextures.at(1) } },
				Model{ "asteroidxlarge3", { &m_rockXLargeTextures.at(2) } },
				Model{ "asteroidxlarge4", { &m_rockXLargeTextures.at(3) } } };
		// Large
		std::array<Model, NUM_LARGE_ASTEROID_MODELS>
			m_asteroidLargeModels{ Model{ "asteroidlarge1", { &m_asteroidLargeTextures.at(0) } },
								   Model{ "asteroidlarge2", { &m_asteroidLargeTextures.at(1) } } };
		std::array<Model, NUM_LARGE_ASTEROID_MODELS>
			m_rockLargeModels{ Model{ "rocklarge1", { &m_rockLargeTextures.at(0) } },
							   Model{ "rocklarge2", { &m_rockLargeTextures.at(1) } } };
		// Medium
		std::array<Model, NUM_MEDIUM_ASTEROID_MODELS>
			m_asteroidMediumModels{	Model{ "asteroidmedium1", { &m_asteroidMediumTextures.at(0) } },
									Model{ "asteroidmedium2", { &m_asteroidMediumTextures.at(1) } } };
		std::array<Model, NUM_MEDIUM_ASTEROID_MODELS>
			m_rockMediumModels{ Model{ "rockmedium1", { &m_rockMediumTextures.at(0) } },
								Model{ "rockmedium2", { &m_rockMediumTextures.at(1) } } };
		// Small
		std::array<Model, NUM_SMALL_ASTEROID_MODELS>
			m_asteroidSmallModels{ Model{ "asteroidsmall1", { &m_asteroidSmallTextures.at(0) } },
								   Model{ "asteroidsmall2", { &m_asteroidSmallTextures.at(1) } } };
		std::array<Model, NUM_SMALL_ASTEROID_MODELS>
			m_rockSmallModels{ Model{ "rocksmall1", { &m_rockSmallTextures.at(0) } },
							   Model{ "rocksmall2", { &m_rockSmallTextures.at(1) } } };
		// Projectiles
		Model m_bulletModel{ "fireball1", { &m_bulletTexture } };
		Model m_missileModel{ "rocket005", 
			{{	d2d::AnimationFrame{ &m_missileTextures.at(0), MISSILE_TIME_PER_FRAME },
				d2d::AnimationFrame{ &m_missileTextures.at(1), MISSILE_TIME_PER_FRAME }},
			d2d::AnimationType::LOOP, 0, true
		} };
		Model m_fatMissileModel{ "rocket006",
			{{	d2d::AnimationFrame{ &m_fatMissileTextures.at(0), MISSILE_TIME_PER_FRAME },
				d2d::AnimationFrame{ &m_fatMissileTextures.at(1), MISSILE_TIME_PER_FRAME }},
			d2d::AnimationType::LOOP, 0, true
		} };

		//+---------------------------\-------------------------------
		//|		  Projectiles		  |
		//\---------------------------/-------------------------------
		// Bullet
		ProjectileDef m_bulletDef{ m_bulletModel,
			BULLET_MATERIAL, { BULLET_HEIGHT * m_bulletTexture.GetWidthToHeightRatio(), BULLET_HEIGHT },
			BULLET_FIXED_ROTATION, BULLET_CONTINUOUS_COLLISION_DETECTION, BULLET_FILTER, 
			BULLET_DESTRUCTION_DELAY, BULLET_DESTRUCTION_DELAY_TIME, 
			BULLET_DESTRUCTION_DELAY_ON_CONTACT, BULLET_DESTRUCTION_DELAY_ON_CONTACT_TIME,
			BULLET_DESTRUCTION_CHANCE_ON_CONTACT, BULLET_DESTRUCTION_CHANCE,
			BULLET_IGNORE_PARENT_COLLISIONS_UNTIL_FIRST_CONTACT, BULLET_ACCELERATION, BULLET_ACCELERATION_TIME };
		// Missile
		ProjectileDef m_missileDef{ m_missileModel,
			MISSILE_MATERIAL, { MISSILE_HEIGHT * m_missileTextures[0].GetWidthToHeightRatio(), MISSILE_HEIGHT },
			MISSILE_FIXED_ROTATION, MISSILE_CONTINUOUS_COLLISION_DETECTION, MISSILE_FILTER, 
			MISSILE_DESTRUCTION_DELAY, MISSILE_DESTRUCTION_DELAY_TIME,
			MISSILE_DESTRUCTION_DELAY_ON_CONTACT, MISSILE_DESTRUCTION_DELAY_ON_CONTACT_TIME,
			MISSILE_DESTRUCTION_CHANCE_ON_CONTACT, MISSILE_DESTRUCTION_CHANCE,
			MISSILE_IGNORE_PARENT_COLLISIONS_UNTIL_FIRST_CONTACT, MISSILE_ACCELERATION, MISSILE_ACCELERATION_TIME };
		// Fat missile
		ProjectileDef m_fatMissileDef{ m_fatMissileModel,
			MISSILE_MATERIAL, { MISSILE_HEIGHT * m_fatMissileTextures[0].GetWidthToHeightRatio(), MISSILE_HEIGHT },
			MISSILE_FIXED_ROTATION, MISSILE_CONTINUOUS_COLLISION_DETECTION, MISSILE_FILTER, 
			MISSILE_DESTRUCTION_DELAY, MISSILE_DESTRUCTION_DELAY_TIME,
			MISSILE_DESTRUCTION_DELAY_ON_CONTACT, MISSILE_DESTRUCTION_DELAY_ON_CONTACT_TIME,
			MISSILE_DESTRUCTION_CHANCE_ON_CONTACT, MISSILE_DESTRUCTION_CHANCE,
			MISSILE_IGNORE_PARENT_COLLISIONS_UNTIL_FIRST_CONTACT, MISSILE_ACCELERATION, MISSILE_ACCELERATION_TIME };
	};
}
