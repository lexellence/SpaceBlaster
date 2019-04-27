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
	constexpr int SHIP_DRAW_LAYER{ 0 };
	constexpr int ASTEROID_DRAW_LAYER{ 0 };

	constexpr float BLASTER_ROTATION_SPEED{ 6.0f };
	constexpr float SCOUT_ROTATION_SPEED{ 6.0f };

	constexpr float BLASTER_THRUSTER_ACCELERATION{ 4.0f };
	constexpr float SCOUT_THRUSTER_ACCELERATION{ 20.0f };

	constexpr float BLASTER_THRUSTER_OFFSET_X	{ -0.44f };
	constexpr float SCOUT_THRUSTER_OFFSET_X		{ -0.9f };
	constexpr float BLASTER_THRUSTER_INNER_SPREAD_Y { 0.33f };
	constexpr float BLASTER_THRUSTER_OUTER_SPREAD_Y { 0.437f };
	constexpr float SCOUT_THRUSTER_SPREAD_Y		{ 0.1f };

	const b2Vec2 BLASTER_THRUSTER_RELATIVE_SIZE{ 0.25f, 0.075f };
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
	constexpr float BULLET_DESTRUCTION_CHANCE{ 0.3f };
	constexpr float MISSILE_DESTRUCTION_CHANCE{ 0.0f };
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

	const d2d::Filter SHIP_FILTER	 { d2d::FILTER_DEFAULT_CATEGORY_BITS, d2d::FILTER_DEFAULT_MASK_BITS, d2d::FILTER_DEFAULT_GROUP_INDEX };
	const d2d::Filter BULLET_FILTER	 { d2d::FILTER_DEFAULT_CATEGORY_BITS, d2d::FILTER_DEFAULT_MASK_BITS, -10 };
	const d2d::Filter MISSILE_FILTER { d2d::FILTER_DEFAULT_CATEGORY_BITS, d2d::FILTER_DEFAULT_MASK_BITS, -10 };
	const d2d::Filter ASTEROID_FILTER{ d2d::FILTER_DEFAULT_CATEGORY_BITS, d2d::FILTER_DEFAULT_MASK_BITS, d2d::FILTER_DEFAULT_GROUP_INDEX };
	
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
		public ProjectileLauncherCallback, 
		public MorphListener
	{
	public:
		Game();
		void Init();
		void Update(float dt, PlayerController& playerController);
		void Draw();

		// World callbacks
		void SayGoodbye(unsigned entityID) override;
		void EntityWrapped(unsigned entityID, const b2Vec2& translation) override;
		unsigned LaunchProjectile(const ProjectileDef& projectileDef, const b2Vec2& position, 
			float angle, float impulse, const b2Vec2& parentVelocity, unsigned parentID) override;
		void MorphedIntoEntity(unsigned replacedEntityID, unsigned newEntityID) override;

	private:
		void InitLevel(const std::string& level);
		void StartDelayedLevelChange(const std::string& level, float delay);
		void UpdateCamera(float dt, const PlayerController& playerController);
		void LoadTest0();
		void LoadTest1();
		void LoadTest2();
		void SetPlayer(unsigned entityID);
		void FollowEntity(unsigned entityID);
		//void CreateEntity(World& world);
		void CreatePlayer(World& world, const b2Vec2& position, float angle);
		unsigned CreateScout(World& world, const b2Vec2& position, float angle, bool activate = true);
		unsigned CreateBlaster(World& world, const b2Vec2& position, float angle, bool activate = true);
		unsigned CreateXLargeAsteroid(World& world, unsigned modelIndex, bool isRock,
			const b2Vec2& position, float angle, const b2Vec2& velocity, float angularVelocity, bool activate = true);
		unsigned CreateLargeAsteroid(World& world, unsigned modelIndex, bool isRock,
			const b2Vec2& position, float angle, const b2Vec2& velocity, float angularVelocity, bool activate = true);
		unsigned CreateMediumAsteroid(World& world, unsigned modelIndex, bool isRock,
			const b2Vec2& position, float angle, const b2Vec2& velocity, float angularVelocity, bool activate = true);
		unsigned CreateSmallAsteroid(World& world, unsigned modelIndex, bool isRock,
			const b2Vec2& position, float angle, const b2Vec2& velocity, float angularVelocity, bool activate = true);
	private:
		GameDef m_settings;
		World m_world;
		Starfield m_starfield;

		Camera m_camera;
		bool m_cameraFollowingEntity{ false };	
		unsigned m_cameraFollowEntityID;

		bool m_playerSet{ false };
		unsigned m_playerID;

		bool m_startOver{ false };
		std::string m_currentLevel;
		bool m_delayedLevelChange{ false };
		std::string m_delayedLevel;
		float m_levelChangeDelayTime;
		float m_levelChangeDelayTimeAccumulator;

		// Fonts
		const d2d::Window::FontReference m_orbitronLightFont{ "Fonts\\OrbitronLight.otf" };

		// Ships
		const std::string m_blasterModel{ "ship001" };
		const d2d::Window::TextureReference m_blasterTexture { "Textures\\BigShips\\ship001.png" };
		const AnimationDef m_blasterAnimationDef{ Texture{ m_blasterTexture.GetID() } };

		const std::string m_scoutModel{ "ship002" };
		const d2d::Window::TextureReference m_scoutTexture { "Textures\\BigShips\\ship002.png" };
		const AnimationDef m_scoutAnimationDef{ Texture{ m_scoutTexture.GetID() } };

		const d2d::Window::TextureReference m_thrusterTexture{ "Textures\\Effects\\thruster1.png" };
		const AnimationDef m_blasterThrusterAnimationDef{ Texture{ m_thrusterTexture.GetID(), BLASTER_THRUSTER_RELATIVE_SIZE } };
		const AnimationDef m_scoutThrusterAnimationDef{ Texture{ m_thrusterTexture.GetID(), SCOUT_THRUSTER_RELATIVE_SIZE } };

		//+------------------------\----------------------------------
		//|	  XL asteroids/rocks   |
		//\------------------------/----------------------------------
		const std::array<std::string, NUM_XLARGE_ASTEROID_MODELS> 
			m_asteroidXLargeModelNames{ "asteroidxlarge1", "asteroidxlarge2", "asteroidxlarge3", "asteroidxlarge4" };
		
		// XL asteroid animations
		const std::array<d2d::Window::TextureReference, NUM_XLARGE_ASTEROID_MODELS> 
			m_asteroidXLargeTextures{ "Textures\\Asteroids\\asteroidxlarge1.png",
									"Textures\\Asteroids\\asteroidxlarge2.png",
									"Textures\\Asteroids\\asteroidxlarge3.png",
									"Textures\\Asteroids\\asteroidxlarge4.png" };
		const std::array<AnimationDef, NUM_XLARGE_ASTEROID_MODELS> 
			m_asteroidXLargeAnimationDefs{ 
				Texture{ m_asteroidXLargeTextures.at(0).GetID() }, 
				Texture{ m_asteroidXLargeTextures.at(1).GetID() },
				Texture{ m_asteroidXLargeTextures.at(2).GetID() },
				Texture{ m_asteroidXLargeTextures.at(3).GetID() } };

		// XL rock animations
		const std::array<d2d::Window::TextureReference, NUM_XLARGE_ASTEROID_MODELS>
			m_rockXLargeTextures{ "Textures\\Asteroids\\rockxlarge1.png", 
								  "Textures\\Asteroids\\rockxlarge2.png", 
								  "Textures\\Asteroids\\rockxlarge3.png", 
								  "Textures\\Asteroids\\rockxlarge4.png" };
		const std::array<AnimationDef, NUM_XLARGE_ASTEROID_MODELS>
			m_rockXLargeAnimationDefs{
				Texture{ m_rockXLargeTextures.at(0).GetID() },
				Texture{ m_rockXLargeTextures.at(1).GetID() },
				Texture{ m_rockXLargeTextures.at(2).GetID() },
				Texture{ m_rockXLargeTextures.at(3).GetID() } };

		//+---------------------------\-------------------------------
		//|	  Large asteroids/rocks   |
		//\---------------------------/-------------------------------
		const std::array<std::string, NUM_LARGE_ASTEROID_MODELS>
			m_asteroidLargeModelNames{ "asteroidlarge1", "asteroidlarge2" };

		// Large asteroid animations
		const std::array<d2d::Window::TextureReference, NUM_LARGE_ASTEROID_MODELS> 
			m_asteroidLargeTextures{ "Textures\\Asteroids\\asteroidlarge1.png", 
									 "Textures\\Asteroids\\asteroidlarge2.png" };
		const std::array<AnimationDef, NUM_LARGE_ASTEROID_MODELS>
			m_asteroidLargeAnimationDefs{
				Texture{ m_asteroidLargeTextures.at(0).GetID() },
				Texture{ m_asteroidLargeTextures.at(1).GetID() } };

		// Large rock animations
		const std::array<d2d::Window::TextureReference, NUM_LARGE_ASTEROID_MODELS> 
			m_rockLargeTextures{ "Textures\\Asteroids\\rocklarge1.png", "Textures\\Asteroids\\rocklarge2.png" };
		const std::array<AnimationDef, NUM_LARGE_ASTEROID_MODELS>
			m_rockLargeAnimationDefs{
				Texture{ m_rockLargeTextures.at(0).GetID() },
				Texture{ m_rockLargeTextures.at(1).GetID() } };

		//+---------------------------\-------------------------------
		//|	 Medium asteroids/rocks   |
		//\---------------------------/-------------------------------
		const std::array<std::string, NUM_MEDIUM_ASTEROID_MODELS>
			m_asteroidMediumModelNames{ "asteroidmedium1", "asteroidmedium2" };

		// Medium asteroid animations
		const std::array<d2d::Window::TextureReference, NUM_MEDIUM_ASTEROID_MODELS>
			m_asteroidMediumTextures{ "Textures\\Asteroids\\asteroidmedium1.png", 
									  "Textures\\Asteroids\\asteroidmedium2.png" };
		const std::array<AnimationDef, NUM_MEDIUM_ASTEROID_MODELS>
			m_asteroidMediumAnimationDefs{
				Texture{ m_asteroidMediumTextures.at(0).GetID() },
				Texture{ m_asteroidMediumTextures.at(1).GetID() } };

		// Medium rock animations
		const std::array<d2d::Window::TextureReference, NUM_MEDIUM_ASTEROID_MODELS>
			m_rockMediumTextures{ "Textures\\Asteroids\\rockmedium1.png", 
								  "Textures\\Asteroids\\rockmedium2.png" };
		const std::array<AnimationDef, NUM_MEDIUM_ASTEROID_MODELS>
			m_rockMediumAnimationDefs{
				Texture{ m_rockMediumTextures.at(0).GetID() },
				Texture{ m_rockMediumTextures.at(1).GetID() } };

		//+---------------------------\-------------------------------
		//|	  Small asteroids/rocks   |
		//\---------------------------/-------------------------------
		const std::array<std::string, NUM_SMALL_ASTEROID_MODELS>
			m_asteroidSmallModelNames{ "asteroidsmall1", "asteroidsmall2" };

		// Small asteroid animations
		const std::array<d2d::Window::TextureReference, NUM_SMALL_ASTEROID_MODELS>
			m_asteroidSmallTextures{ "Textures\\Asteroids\\asteroidsmall1.png",
		"Textures\\Asteroids\\asteroidsmall2.png" };
		const std::array<AnimationDef, NUM_SMALL_ASTEROID_MODELS>
			m_asteroidSmallAnimationDefs{
				Texture{ m_asteroidSmallTextures.at(0).GetID() },
				Texture{ m_asteroidSmallTextures.at(1).GetID() } };

		// Small rock animations
		const std::array<d2d::Window::TextureReference, NUM_SMALL_ASTEROID_MODELS>
			m_rockSmallTextures{ "Textures\\Asteroids\\rocksmall1.png", 
								 "Textures\\Asteroids\\rocksmall2.png" };
		const std::array<AnimationDef, NUM_SMALL_ASTEROID_MODELS>
			m_rockSmallAnimationDefs{
				Texture{ m_rockSmallTextures.at(0).GetID() },
				Texture{ m_rockSmallTextures.at(1).GetID() } };

		//+---------------------------\-------------------------------
		//|		  Projectiles		  |
		//\---------------------------/-------------------------------
		d2d::Window::TextureReference m_bulletTexture{ "Textures\\Projectiles\\fireball1.png" };
		AnimationDef m_bulletAnimationDef{ Texture{ m_bulletTexture.GetID() } };
		const ProjectileDef m_bulletDef{ &m_bulletAnimationDef, "fireball1",
			BULLET_MATERIAL, { BULLET_HEIGHT * m_bulletTexture.GetWidthToHeightRatio(), BULLET_HEIGHT },
			BULLET_FIXED_ROTATION, BULLET_CONTINUOUS_COLLISION_DETECTION, BULLET_FILTER, 
			BULLET_DESTRUCTION_DELAY, BULLET_DESTRUCTION_DELAY_TIME, 
			BULLET_DESTRUCTION_DELAY_ON_CONTACT, BULLET_DESTRUCTION_DELAY_ON_CONTACT_TIME,
			BULLET_DESTRUCTION_CHANCE_ON_CONTACT, BULLET_DESTRUCTION_CHANCE,
			BULLET_IGNORE_PARENT_COLLISIONS_UNTIL_FIRST_CONTACT, BULLET_ACCELERATION, BULLET_ACCELERATION_TIME };

		const std::array<d2d::Window::TextureReference, 2>
			m_missileTextures{ "Textures\\Projectiles\\rocket005a.png",
							   "Textures\\Projectiles\\rocket005b.png" };
		const AnimationDef m_missileAnimationDef{ AnimationType::LOOP, m_missileTextures.size(), 0, true,
			FrameDefArray{ FrameDef{ MISSILE_TIME_PER_FRAME, Texture{ m_missileTextures[0].GetID() } },
						   FrameDef{ MISSILE_TIME_PER_FRAME, Texture{ m_missileTextures[1].GetID() } } } };
		const ProjectileDef m_missileDef{ &m_missileAnimationDef, "rocket005",
			MISSILE_MATERIAL, { MISSILE_HEIGHT * m_missileTextures[0].GetWidthToHeightRatio(), MISSILE_HEIGHT },
			MISSILE_FIXED_ROTATION, MISSILE_CONTINUOUS_COLLISION_DETECTION, MISSILE_FILTER, 
			MISSILE_DESTRUCTION_DELAY, MISSILE_DESTRUCTION_DELAY_TIME,
			MISSILE_DESTRUCTION_DELAY_ON_CONTACT, MISSILE_DESTRUCTION_DELAY_ON_CONTACT_TIME,
			MISSILE_DESTRUCTION_CHANCE_ON_CONTACT, MISSILE_DESTRUCTION_CHANCE,
			MISSILE_IGNORE_PARENT_COLLISIONS_UNTIL_FIRST_CONTACT, MISSILE_ACCELERATION, MISSILE_ACCELERATION_TIME };

		const std::array<d2d::Window::TextureReference, 2>
			m_fatMissileTextures{ "Textures\\Projectiles\\rocket006a.png",
							      "Textures\\Projectiles\\rocket006b.png" };
		const AnimationDef m_fatMissileAnimationDef{ AnimationType::LOOP, m_fatMissileTextures.size(), 0, true,
			FrameDefArray{ FrameDef{ MISSILE_TIME_PER_FRAME, Texture{ m_fatMissileTextures[0].GetID() } },
						   FrameDef{ MISSILE_TIME_PER_FRAME, Texture{ m_fatMissileTextures[1].GetID() } } } };
		const ProjectileDef m_fatMissileDef{ &m_fatMissileAnimationDef, "rocket006",
			MISSILE_MATERIAL, { MISSILE_HEIGHT * m_fatMissileTextures[0].GetWidthToHeightRatio(), MISSILE_HEIGHT },
			MISSILE_FIXED_ROTATION, MISSILE_CONTINUOUS_COLLISION_DETECTION, MISSILE_FILTER, 
			MISSILE_DESTRUCTION_DELAY, MISSILE_DESTRUCTION_DELAY_TIME,
			MISSILE_DESTRUCTION_DELAY_ON_CONTACT, MISSILE_DESTRUCTION_DELAY_ON_CONTACT_TIME,
			MISSILE_DESTRUCTION_CHANCE_ON_CONTACT, MISSILE_DESTRUCTION_CHANCE,
			MISSILE_IGNORE_PARENT_COLLISIONS_UNTIL_FIRST_CONTACT, MISSILE_ACCELERATION, MISSILE_ACCELERATION_TIME };
	};
}
