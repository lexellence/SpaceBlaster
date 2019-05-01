/**************************************************************************************\
** File: Game.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the Game class
**
\**************************************************************************************/
#include "pch.h"
#include "Game.h"
#include "GameModelData.h"
#include "World.h"
#include "Camera.h"
#include "Starfield.h"
#include "Exceptions.h"

namespace Space
{
	//+-------------\---------------------------------------------
	//|	  Levels    |
	//\-------------/---------------------------------------------
	Game::Game()
	{
		m_world.SetDestructionListener(this);
		m_world.SetWrapListener(this);
		m_world.SetProjectileLauncherCallback(this);
		m_world.SetMorphListener(this);
	}
	void Game::Init()
	{
		m_settings.LoadFrom("Data\\game.hjson");
		InitLevel(m_settings.firstLevel);
	}
	void Game::InitLevel(const std::string& level)
	{
		m_camera.Init(m_settings.cameraDimensionRange, m_settings.cameraZoomSpeed, m_settings.cameraInitialZoomOutPercent);
		m_cameraFollowingEntity = false;
		m_starfield.Init(m_settings.starfield);
		m_playerSet = false;
		m_startOver = false;
		m_currentLevel = level;
		m_delayedLevelChange = false;

		if(level == "TEST0")
			LoadTest0();
		else if(level == "TEST1")
			LoadTest1();
		else if(level == "TEST2")
			LoadTest2();
		else
			throw GameException{ "Invalid level name: " + level };
	}
	void Game::SetPlayer(unsigned entityID)
	{
		if(m_world.EntityExists(entityID))
		{
			// Remove player controller from existing player entity
			if(m_playerSet)
				m_world.SetFlags(m_playerID, FLAG_PLAYER_CONTROLLED, false);			

			// Set new player
			m_world.SetFlags(entityID, FLAG_PLAYER_CONTROLLED, true);
			m_playerID = entityID;
			m_playerSet = true;
		}
	}
	void Game::FollowEntity(unsigned entityID)
	{
		m_cameraFollowingEntity = true;
		m_cameraFollowEntityID = entityID;
	}
	void Game::LoadTest0()
	{
		b2Vec2 center{ b2Vec2_zero };
		b2Vec2 size{ 100.0f, 100.0f };
		d2d::Rect worldRect;
		worldRect.SetCenter(center, size);
		m_world.Init(worldRect);
		b2Vec2 playerStartPosition{ b2Vec2_zero };
		float playerStartRotationPercent{ 0.25f };
		CreatePlayer(m_world, playerStartPosition, playerStartRotationPercent * d2d::TWO_PI);
		CreateXLargeAsteroid(m_world, 0, false, playerStartPosition + b2Vec2{ 0.0f, 10.0f }, 0.0f, b2Vec2_zero, 0.0f);
	}
	void Game::LoadTest1()
	{
		b2Vec2 center{ b2Vec2_zero };
		b2Vec2 size{ 500.0f, 500.0f };
		d2d::Rect worldRect;
		worldRect.SetCenter(center, size);
		m_world.Init(worldRect);

		b2Vec2 playerStartPosition{ b2Vec2_zero };
		CreatePlayer(m_world, playerStartPosition, d2d::PI_OVER_TWO);

		float minBoundingRadiiGap{ XLARGE_ASTEROID_HEIGHT * 0.25f };
		unsigned maxAttempts{ 500 };
		unsigned numFailed{ 0 };
		for(unsigned i = 0; i < 20; ++i)
		{
			int model{ d2d::RandomInt({0, NUM_XLARGE_ASTEROID_MODELS - 1}) };
			b2Vec2 size;
			size.y = XLARGE_ASTEROID_HEIGHT * XLARGE_ASTEROID_RELATIVE_HEIGHTS[model];
			size.x = size.y * m_asteroidXLargeTextures[model].GetWidthToHeightRatio();
			float boundingRadius{ size.Length() * 0.5f };
			b2Vec2 position;
			bool positionFound{ m_world.GetRandomPositionAwayFromExistingEntities(boundingRadius, minBoundingRadiiGap, maxAttempts, position) };
			if(positionFound)
				CreateXLargeAsteroid(m_world, model, d2d::RandomBool(), position,
					d2d::RandomFloat({ 0.0f, d2d::TWO_PI }), b2Vec2_zero, 0.0f);
			else
				++numFailed;
		}
		minBoundingRadiiGap = LARGE_ASTEROID_HEIGHT * 0.25f;
		for(unsigned i = 0; i < 30; ++i)
		{
			int model{ d2d::RandomInt({0, NUM_LARGE_ASTEROID_MODELS - 1}) };
			b2Vec2 size;
			size.y = LARGE_ASTEROID_HEIGHT * LARGE_ASTEROID_RELATIVE_HEIGHTS[model];
			size.x = size.y * m_asteroidLargeTextures[model].GetWidthToHeightRatio();
			float boundingRadius{ size.Length() * 0.5f };
			b2Vec2 position;
			bool positionFound{ m_world.GetRandomPositionAwayFromExistingEntities(boundingRadius, minBoundingRadiiGap, maxAttempts, position) };
			if(positionFound)
				CreateLargeAsteroid(m_world, model, d2d::RandomBool(), position,
					d2d::RandomFloat({ 0.0f, d2d::TWO_PI }), b2Vec2_zero, 0.0f);
			else
				++numFailed;
		}
		minBoundingRadiiGap = MEDIUM_ASTEROID_HEIGHT * 0.25f;
		for(unsigned i = 0; i < 40; ++i)
		{
			int model{ d2d::RandomInt({0, NUM_MEDIUM_ASTEROID_MODELS - 1}) };
			b2Vec2 size;
			size.y = MEDIUM_ASTEROID_HEIGHT * MEDIUM_ASTEROID_RELATIVE_HEIGHTS[model];
			size.x = size.y * m_asteroidMediumTextures[model].GetWidthToHeightRatio();
			float boundingRadius{ size.Length() * 0.5f };
			b2Vec2 position;
			bool positionFound{ m_world.GetRandomPositionAwayFromExistingEntities(boundingRadius, minBoundingRadiiGap, maxAttempts, position) };
			if(positionFound)
				CreateMediumAsteroid(m_world, model, d2d::RandomBool(), position,
					d2d::RandomFloat({ 0.0f, d2d::TWO_PI }), b2Vec2_zero, 0.0f);
			else
				++numFailed;
		}
		minBoundingRadiiGap = SMALL_ASTEROID_HEIGHT * 0.25f;
		for(unsigned i = 0; i < 100; ++i)
		{
			int model{ d2d::RandomInt({0, NUM_SMALL_ASTEROID_MODELS - 1}) };
			b2Vec2 size;
			size.y = SMALL_ASTEROID_HEIGHT * SMALL_ASTEROID_RELATIVE_HEIGHTS[model];
			size.x = size.y * m_asteroidSmallTextures[model].GetWidthToHeightRatio();
			float boundingRadius{ size.Length() * 0.5f };
			b2Vec2 position;
			bool positionFound{ m_world.GetRandomPositionAwayFromExistingEntities(boundingRadius, minBoundingRadiiGap, maxAttempts, position) };
			if(positionFound)
				CreateSmallAsteroid(m_world, model, d2d::RandomBool(), position,
					d2d::RandomFloat({ 0.0f, d2d::TWO_PI }), b2Vec2_zero, 0.0f);
			else
				++numFailed;
		}

		if(numFailed > 0)
			d2LogInfo << "Game::LoadTest1: Entity creation attempt limit reached " << numFailed << " times";
	}
	void Game::LoadTest2()
	{
		b2Vec2 center{ b2Vec2_zero };
		b2Vec2 size{ 1000.0f, 1000.0f };
		d2d::Rect worldRect;
		worldRect.SetCenter(center, size);
		m_world.Init(worldRect);

		b2Vec2 playerStartPosition{ b2Vec2_zero };
		CreatePlayer(m_world, playerStartPosition, d2d::PI_OVER_TWO);
		for(unsigned j = 0; j < 2; ++j)
		{
			bool isRock{ (bool)j };
			for(unsigned model = 0; model < NUM_XLARGE_ASTEROID_MODELS; ++model)
				CreateXLargeAsteroid(m_world, model, isRock, playerStartPosition + b2Vec2{ j * 75.0f, (model + 1) * 20.0f }, 0.0f, b2Vec2_zero, 0.0f);
			for(unsigned model = 0; model < NUM_LARGE_ASTEROID_MODELS; ++model)
				CreateLargeAsteroid(m_world, model, isRock, playerStartPosition + b2Vec2{ 8.0f + j * 75.0f, (model + 1) * 20.0f }, 0.0f, b2Vec2_zero, 0.0f);
			for(unsigned model = 0; model < NUM_MEDIUM_ASTEROID_MODELS; ++model)
				CreateMediumAsteroid(m_world, model, isRock, playerStartPosition + b2Vec2{ 16.0f + j * 75.0f, (model + 1) * 20.0f }, 0.0f, b2Vec2_zero, 0.0f);
			for(unsigned model = 0; model < NUM_SMALL_ASTEROID_MODELS; ++model)
				CreateSmallAsteroid(m_world, model, isRock, playerStartPosition + b2Vec2{ 24.0f + j * 75.0f, (model + 1) * 20.0f }, 0.0f, b2Vec2_zero, 0.0f);
		}
	}
	//+-------------\---------------------------------------------
	//|	 Entities   |
	//\-------------/---------------------------------------------
	/*void Game::CreateEntity(World& world)
	{

	}*/
	void Game::CreatePlayer(World& world, const b2Vec2& position, float angle)
	{
		unsigned blasterID{ CreateBlaster(world, position, d2d::PI_OVER_TWO, true) };
		unsigned scoutID{ CreateScout(world, position, d2d::PI_OVER_TWO, false) };
		world.AddMorphIntoEntityID(blasterID, scoutID);
		world.AddMorphIntoEntityID(scoutID, blasterID);
		SetPlayer(blasterID);
		FollowEntity(blasterID);
	}
	unsigned Game::CreateScout(World& world, const b2Vec2& position, float angle, bool activate)
	{
		unsigned id;
		{
			b2Vec2 size{ SCOUT_HEIGHT * m_scoutTexture.GetWidthToHeightRatio(), SCOUT_HEIGHT };
			id = m_world.NewEntityID(size, SHIP_DRAW_LAYER, activate);
		}
		world.AddPhysicsComponent(id, b2_dynamicBody, position, angle);
		world.AddShapes(id, m_scoutModel, SHIP_MATERIAL, SHIP_FILTER);
		world.AddDrawAnimationComponent(id, &m_scoutAnimationDef);
		world.AddRotatorComponent(id, SCOUT_ROTATION_SPEED);
		
		world.AddThrusterComponent(id, 2);
		world.AddThruster(id, 0, &m_scoutThrusterAnimationDef, SCOUT_THRUSTER_ACCELERATION, { SCOUT_THRUSTER_OFFSET_X,  SCOUT_THRUSTER_SPREAD_Y });
		world.AddThruster(id, 1, &m_scoutThrusterAnimationDef, SCOUT_THRUSTER_ACCELERATION, { SCOUT_THRUSTER_OFFSET_X, -SCOUT_THRUSTER_SPREAD_Y });
		
		world.AddBrakeComponent(id, SCOUT_BRAKE_DECELERATION);
			
		// Bullets
		world.AddProjectileLauncherComponent(id, 3, false);
		world.AddProjectileLauncher(id, 0, m_bulletDef, { SCOUT_PROJECTILE_OFFSET_X, 0.0f }, SCOUT_CANON_IMPULSE, SCOUT_CANON_INTERVAL, false, false);
		world.AddProjectileLauncher(id, 1, m_bulletDef, { SCOUT_PROJECTILE_OFFSET_X,  SCOUT_PROJECTILE_SPREAD_Y }, SCOUT_CANON_IMPULSE, SCOUT_CANON_INTERVAL, false, false);
		world.AddProjectileLauncher(id, 2, m_bulletDef, { SCOUT_PROJECTILE_OFFSET_X, -SCOUT_PROJECTILE_SPREAD_Y }, SCOUT_CANON_IMPULSE, SCOUT_CANON_INTERVAL, false, false);

		world.AddHealthComponent(id, SCOUT_HP);
		world.AddParticleExplosionOnDeathComponent(id, PARTICLE_EXPLOSION_RELATIVE_SIZE,
			SCOUT_NUM_PARTICLES, SCOUT_PARTICLE_SPEED_RANGE, DAMAGE_BASED_SPEED_INCREASE_FACTOR,
			SCOUT_PARTICLE_SIZE_INDEX_RANGE, BLASTER_PARTICLE_COLOR_RANGE,
			SCOUT_PARTICLE_LIFETIME, PARTICLE_EXPLOSION_FADEIN, SCOUT_PARTICLE_FADEOUT);
		return id;
	}
	unsigned Game::CreateBlaster(World& world, const b2Vec2& position, float angle, bool activate)
	{
		unsigned id;
		{
			b2Vec2 size{ BLASTER_HEIGHT * m_blasterTexture.GetWidthToHeightRatio(), BLASTER_HEIGHT };
			id = m_world.NewEntityID(size, SHIP_DRAW_LAYER, activate);
		}
		world.AddPhysicsComponent(id, b2_dynamicBody, position, angle);
		world.AddShapes(id, m_blasterModel, SHIP_MATERIAL, SHIP_FILTER);
		world.AddDrawAnimationComponent(id, &m_blasterAnimationDef);
		world.AddRotatorComponent(id, BLASTER_ROTATION_SPEED);

		world.AddThrusterComponent(id, 4);
		world.AddThruster(id, 0, &m_blasterThrusterAnimationDef, BLASTER_THRUSTER_ACCELERATION, { BLASTER_THRUSTER_OFFSET_X,  BLASTER_THRUSTER_INNER_SPREAD_Y });
		world.AddThruster(id, 1, &m_blasterThrusterAnimationDef, BLASTER_THRUSTER_ACCELERATION, { BLASTER_THRUSTER_OFFSET_X, -BLASTER_THRUSTER_INNER_SPREAD_Y });
		world.AddThruster(id, 2, &m_blasterThrusterAnimationDef, BLASTER_THRUSTER_ACCELERATION, { BLASTER_THRUSTER_OFFSET_X,  BLASTER_THRUSTER_OUTER_SPREAD_Y });
		world.AddThruster(id, 3, &m_blasterThrusterAnimationDef, BLASTER_THRUSTER_ACCELERATION, { BLASTER_THRUSTER_OFFSET_X, -BLASTER_THRUSTER_OUTER_SPREAD_Y });
		
		world.AddBrakeComponent(id, BLASTER_BRAKE_DECELERATION);
			
		// Bullets
		world.AddProjectileLauncherComponent(id, 5, false);
		world.AddProjectileLauncher(id, 0, m_bulletDef, { BLASTER_PROJECTILE_OFFSET_X, 0.0f }, BLASTER_CANON_IMPULSE, BLASTER_CANON_INTERVAL, false, false);
		world.AddProjectileLauncher(id, 1, m_bulletDef, { BLASTER_PROJECTILE_OFFSET_X,  BLASTER_PROJECTILE_INNER_SPREAD_Y }, BLASTER_CANON_IMPULSE, BLASTER_CANON_INTERVAL, false, false);
		world.AddProjectileLauncher(id, 2, m_bulletDef, { BLASTER_PROJECTILE_OFFSET_X, -BLASTER_PROJECTILE_INNER_SPREAD_Y }, BLASTER_CANON_IMPULSE, BLASTER_CANON_INTERVAL, false, false);
		world.AddProjectileLauncher(id, 3, m_bulletDef, { BLASTER_PROJECTILE_OFFSET_X,  BLASTER_PROJECTILE_OUTER_SPREAD_Y }, BLASTER_CANON_IMPULSE, BLASTER_CANON_INTERVAL, false, false);
		world.AddProjectileLauncher(id, 4, m_bulletDef, { BLASTER_PROJECTILE_OFFSET_X, -BLASTER_PROJECTILE_OUTER_SPREAD_Y }, BLASTER_CANON_IMPULSE, BLASTER_CANON_INTERVAL, false, false);
			
		// Missiles
		world.AddProjectileLauncherComponent(id, 5, true);
		world.AddProjectileLauncher(id, 0, m_fatMissileDef, { BLASTER_PROJECTILE_OFFSET_X, 0.0f }, BLASTER_MISSILE_LAUNCHER_IMPULSE, BLASTER_MISSILE_LAUNCHER_INTERVAL, false, true);
		world.AddProjectileLauncher(id, 1, m_missileDef, { BLASTER_PROJECTILE_OFFSET_X,  BLASTER_PROJECTILE_INNER_SPREAD_Y }, BLASTER_MISSILE_LAUNCHER_IMPULSE, BLASTER_MISSILE_LAUNCHER_INTERVAL, false, true);
		world.AddProjectileLauncher(id, 2, m_missileDef, { BLASTER_PROJECTILE_OFFSET_X, -BLASTER_PROJECTILE_INNER_SPREAD_Y }, BLASTER_MISSILE_LAUNCHER_IMPULSE, BLASTER_MISSILE_LAUNCHER_INTERVAL, false, true);
		world.AddProjectileLauncher(id, 3, m_missileDef, { BLASTER_PROJECTILE_OFFSET_X,  BLASTER_PROJECTILE_OUTER_SPREAD_Y }, BLASTER_MISSILE_LAUNCHER_IMPULSE, BLASTER_MISSILE_LAUNCHER_INTERVAL, false, true);
		world.AddProjectileLauncher(id, 4, m_missileDef, { BLASTER_PROJECTILE_OFFSET_X, -BLASTER_PROJECTILE_OUTER_SPREAD_Y }, BLASTER_MISSILE_LAUNCHER_IMPULSE, BLASTER_MISSILE_LAUNCHER_INTERVAL, false, true);

		world.AddHealthComponent(id, BLASTER_HP);
		world.AddParticleExplosionOnDeathComponent(id, PARTICLE_EXPLOSION_RELATIVE_SIZE, 
			BLASTER_NUM_PARTICLES, BLASTER_PARTICLE_SPEED_RANGE, DAMAGE_BASED_SPEED_INCREASE_FACTOR,
			BLASTER_PARTICLE_SIZE_INDEX_RANGE, BLASTER_PARTICLE_COLOR_RANGE,
			BLASTER_PARTICLE_LIFETIME, PARTICLE_EXPLOSION_FADEIN, BLASTER_PARTICLE_FADEOUT);
		return id;
	}
	unsigned Game::CreateXLargeAsteroid(World& world, unsigned modelIndex, bool isRock,
		const b2Vec2& position, float angle, const b2Vec2& velocity, float angularVelocity, bool activate)
	{
		d2Assert(modelIndex < NUM_XLARGE_ASTEROID_MODELS);
		unsigned id;
		{
			float height{ XLARGE_ASTEROID_HEIGHT * XLARGE_ASTEROID_RELATIVE_HEIGHTS[modelIndex] };
			b2Vec2 size{ height * m_asteroidXLargeTextures[modelIndex].GetWidthToHeightRatio(), height };
			id = m_world.NewEntityID(size, ASTEROID_DRAW_LAYER, activate);
		}
		world.AddPhysicsComponent(id, b2_dynamicBody,	position, angle, velocity, angularVelocity);
		world.AddShapes(id, m_asteroidXLargeModelNames[modelIndex], ASTEROID_MATERIAL, ASTEROID_FILTER);
		world.AddDrawAnimationComponent(id, isRock ? &m_rockXLargeAnimationDefs[modelIndex] : &m_asteroidXLargeAnimationDefs[modelIndex]);
		world.AddHealthComponent(id, XLARGE_ASTEROID_HP);
		world.AddParticleExplosionOnDeathComponent(id, PARTICLE_EXPLOSION_RELATIVE_SIZE,
			XLARGE_ASTEROID_NUM_PARTICLES, ASTEROID_PARTICLE_SPEED_RANGE, DAMAGE_BASED_SPEED_INCREASE_FACTOR,
			XLARGE_ASTEROID_PARTICLE_SIZE_INDEX_RANGE, isRock ? ROCK_PARTICLE_COLOR_RANGE : ASTEROID_PARTICLE_COLOR_RANGE,
			XLARGE_ASTEROID_PARTICLE_LIFETIME, PARTICLE_EXPLOSION_FADEIN, XLARGE_ASTEROID_PARTICLE_FADEOUT);
		return id;
	}
	unsigned Game::CreateLargeAsteroid(World& world, unsigned modelIndex, bool isRock,
		const b2Vec2& position, float angle, const b2Vec2& velocity, float angularVelocity, bool activate)
	{
		d2Assert(modelIndex < NUM_LARGE_ASTEROID_MODELS);
		unsigned id;
		{
			float height{ LARGE_ASTEROID_HEIGHT * LARGE_ASTEROID_RELATIVE_HEIGHTS[modelIndex] };
			b2Vec2 size{ height * m_asteroidLargeTextures[modelIndex].GetWidthToHeightRatio(), height };
			id = m_world.NewEntityID(size, ASTEROID_DRAW_LAYER, activate);
		}
		world.AddPhysicsComponent(id, b2_dynamicBody, position, angle, velocity, angularVelocity);
		world.AddShapes(id, m_asteroidLargeModelNames[modelIndex], ASTEROID_MATERIAL, ASTEROID_FILTER);
		world.AddDrawAnimationComponent(id, isRock ? &m_rockLargeAnimationDefs[modelIndex] : &m_asteroidLargeAnimationDefs[modelIndex]);
		world.AddHealthComponent(id, LARGE_ASTEROID_HP);
		world.AddParticleExplosionOnDeathComponent(id, PARTICLE_EXPLOSION_RELATIVE_SIZE,
			LARGE_ASTEROID_NUM_PARTICLES, ASTEROID_PARTICLE_SPEED_RANGE, DAMAGE_BASED_SPEED_INCREASE_FACTOR,
			LARGE_ASTEROID_PARTICLE_SIZE_INDEX_RANGE, isRock ? ROCK_PARTICLE_COLOR_RANGE : ASTEROID_PARTICLE_COLOR_RANGE,
			LARGE_ASTEROID_PARTICLE_LIFETIME, PARTICLE_EXPLOSION_FADEIN, LARGE_ASTEROID_PARTICLE_FADEOUT);
		return id;
	}
	unsigned Game::CreateMediumAsteroid(World& world, unsigned modelIndex, bool isRock,
		const b2Vec2& position, float angle, const b2Vec2& velocity, float angularVelocity, bool activate)
	{
		d2Assert(modelIndex < NUM_MEDIUM_ASTEROID_MODELS);
		unsigned id;
		{
			float height{ MEDIUM_ASTEROID_HEIGHT * MEDIUM_ASTEROID_RELATIVE_HEIGHTS[modelIndex] };
			b2Vec2 size{ height * m_asteroidMediumTextures[modelIndex].GetWidthToHeightRatio(), height };
			id = m_world.NewEntityID(size, ASTEROID_DRAW_LAYER, activate);
		}
		world.AddPhysicsComponent(id, b2_dynamicBody, position, angle, velocity, angularVelocity);
		world.AddShapes(id, m_asteroidMediumModelNames[modelIndex], ASTEROID_MATERIAL, ASTEROID_FILTER);
		world.AddDrawAnimationComponent(id, isRock ? &m_rockMediumAnimationDefs[modelIndex] : &m_asteroidMediumAnimationDefs[modelIndex]);
		world.AddHealthComponent(id, MEDIUM_ASTEROID_HP);
		world.AddParticleExplosionOnDeathComponent(id, PARTICLE_EXPLOSION_RELATIVE_SIZE,
			MEDIUM_ASTEROID_NUM_PARTICLES, ASTEROID_PARTICLE_SPEED_RANGE, DAMAGE_BASED_SPEED_INCREASE_FACTOR,
			MEDIUM_ASTEROID_PARTICLE_SIZE_INDEX_RANGE, isRock ? ROCK_PARTICLE_COLOR_RANGE : ASTEROID_PARTICLE_COLOR_RANGE,
			MEDIUM_ASTEROID_PARTICLE_LIFETIME, PARTICLE_EXPLOSION_FADEIN, MEDIUM_ASTEROID_PARTICLE_FADEOUT);
		return id;
	}
	unsigned Game::CreateSmallAsteroid(World& world, unsigned modelIndex, bool isRock,
		const b2Vec2& position, float angle, const b2Vec2& velocity, float angularVelocity, bool activate)
	{
		d2Assert(modelIndex < NUM_SMALL_ASTEROID_MODELS);
		unsigned id;
		{
			float height{ SMALL_ASTEROID_HEIGHT * SMALL_ASTEROID_RELATIVE_HEIGHTS[modelIndex] };
			b2Vec2 size{ height * m_asteroidSmallTextures[modelIndex].GetWidthToHeightRatio(), height };
			id = m_world.NewEntityID(size, ASTEROID_DRAW_LAYER, activate);
		}
		world.AddPhysicsComponent(id, b2_dynamicBody, position, angle, velocity, angularVelocity);
		world.AddShapes(id, m_asteroidSmallModelNames[modelIndex], ASTEROID_MATERIAL, ASTEROID_FILTER);
		world.AddDrawAnimationComponent(id, isRock ? &m_rockSmallAnimationDefs[modelIndex] : &m_asteroidSmallAnimationDefs[modelIndex]);
		world.AddHealthComponent(id, SMALL_ASTEROID_HP);
		world.AddParticleExplosionOnDeathComponent(id, PARTICLE_EXPLOSION_RELATIVE_SIZE,
			SMALL_ASTEROID_NUM_PARTICLES, ASTEROID_PARTICLE_SPEED_RANGE, DAMAGE_BASED_SPEED_INCREASE_FACTOR,
			SMALL_ASTEROID_PARTICLE_SIZE_INDEX_RANGE, isRock ? ROCK_PARTICLE_COLOR_RANGE : ASTEROID_PARTICLE_COLOR_RANGE,
			SMALL_ASTEROID_PARTICLE_LIFETIME, PARTICLE_EXPLOSION_FADEIN, SMALL_ASTEROID_PARTICLE_FADEOUT);
		return id;
	}
	//+-------------\---------------------------------------------
	//|	  Update    |
	//\-------------/---------------------------------------------
	void Game::Update(float dt, PlayerController& playerController)
	{
		// Delayed level change
		if(m_delayedLevelChange)
		{
			m_levelChangeDelayTimeAccumulator += dt;
			if(m_levelChangeDelayTimeAccumulator >= m_levelChangeDelayTime)
			{
				dt = m_levelChangeDelayTimeAccumulator - m_levelChangeDelayTime;
				InitLevel(m_delayedLevel);
			}
		}

		// Update
		m_world.Update(dt, playerController);
		UpdateCamera(dt, playerController);
	}
	void Game::StartDelayedLevelChange(const std::string& level, float delay)
	{
		m_delayedLevelChange = true;
		m_delayedLevel = level;
		m_levelChangeDelayTime = delay;
		m_levelChangeDelayTimeAccumulator = 0.0f;
	}
	void Game::UpdateCamera(float dt, const PlayerController& playerController)
	{
		if(m_cameraFollowingEntity)
			if(m_world.HasPhysics(m_cameraFollowEntityID))
				m_camera.SetPosition(b2Mul( m_world.GetSmoothedTransform(m_cameraFollowEntityID), 
										    m_world.GetLocalCenterOfMass(m_cameraFollowEntityID) ));
		m_camera.Update(dt, playerController.zoomOutFactor);
	}
	//+------------------------\----------------------------------
	//|	  World Callbacks	   |
	//\------------------------/----------------------------------
	void Game::SayGoodbye(unsigned entityID)
	{
		if(m_cameraFollowingEntity && entityID == m_cameraFollowEntityID)
			m_cameraFollowingEntity = false;

		if(m_playerSet && entityID == m_playerID)
		{
			StartDelayedLevelChange(m_currentLevel, 3.0f);
			m_playerSet = false;
		}
	}
	void Game::EntityWrapped(unsigned entityID, const b2Vec2& translation)
	{
		if(m_cameraFollowingEntity && entityID == m_cameraFollowEntityID)
			m_starfield.Translate(translation);
	}
	unsigned Game::LaunchProjectile(const ProjectileDef& projectileDef, const b2Vec2& position,
		float angle, float impulse, const b2Vec2& parentVelocity, unsigned parentID)
	{
		unsigned id{ m_world.NewEntityID(projectileDef.dimensions, m_world.GetDrawLayer(parentID) - 1, true) };
		m_world.AddPhysicsComponent(id, b2_dynamicBody,
			position, angle, parentVelocity, 0.0f,
			projectileDef.fixedRotation, projectileDef.continuousCollisionDetection);

		m_world.AddShapes(id, projectileDef.modelName, projectileDef.material, projectileDef.filter);

		if(impulse > 0.0f)
		{
			b2Vec2 unitDirectionVector{ cos(angle), sin(angle) };
			m_world.ApplyLinearImpulseToCenter(id, impulse * unitDirectionVector);
		}

		m_world.AddDrawAnimationComponent(id, projectileDef.animationDefPtr);

		if(projectileDef.destructionDelay)
			m_world.AddDestructionDelayComponent(id, projectileDef.destructionDelayTime);

		if(projectileDef.destructionDelayOnContact)
			m_world.AddDestructionDelayOnContactComponent(id, projectileDef.destructionDelayOnContactTime);

		if(projectileDef.destructionChanceOnContact)
			m_world.AddDestructionChanceOnContactComponent(id, projectileDef.destructionChance);

		m_world.AddParentComponent(id, parentID);
		if(projectileDef.ignoreParentCollisionsUntilFirstContactEnd)
			m_world.SetFlags(id, FLAG_IGNORE_PARENT_COLLISIONS_UNTIL_FIRST_CONTACT_END);

		if(projectileDef.acceleration > 0.0f && projectileDef.accelerationTime > 0.0f)
		{
			m_world.AddThrusterComponent(id, 1, 1.0f);
			m_world.AddThruster(id, 0, nullptr, projectileDef.acceleration, b2Vec2_zero);
			m_world.AddSetThrustFactorAfterDelayComponent(id, 0.0f, projectileDef.accelerationTime);
		}
		return id;
	}
	void Game::MorphedIntoEntity(unsigned replacedEntityID, unsigned newEntityID)
	{
		if(m_playerSet && replacedEntityID == m_playerID)
			SetPlayer(newEntityID);
		if(m_cameraFollowingEntity && replacedEntityID == m_cameraFollowEntityID)
			FollowEntity(newEntityID);
	}
	//+-------------\---------------------------------------------
	//|	   Draw     |
	//\-------------/---------------------------------------------
	void Game::Draw()
	{
		d2d::Rect cameraRect;
		cameraRect.SetCenter(m_camera.GetPosition(), m_camera.GetDimensions(d2d::Window::GetXYAspectRatio()));
		d2d::Window::SetCameraRect(cameraRect);

		m_starfield.Draw(m_camera.GetPosition());
		m_world.Draw();
	}
}