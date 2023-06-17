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
	}
	void Game::Init()
	{
		m_settings.LoadFrom("Data/game.hjson");
		StartCurrentLevel();
	}
	void Game::StartCurrentLevel()
	{
		ClearLevel({ 500.0f, 500.0f });
		SpawnPlayer();
		SpawnExit();

		unsigned numFailed{ 0 };
		{
			unsigned numToCreate = 8;
			numFailed += numToCreate - SpawnRandomIcons(numToCreate);

			numToCreate = 20;
			numFailed += numToCreate - SpawnRandomXLargeAsteroids(numToCreate);
			numToCreate = 30;
			numFailed += numToCreate - SpawnRandomLargeAsteroids(numToCreate);
			numToCreate = 40;
			numFailed += numToCreate - SpawnRandomMediumAsteroids(numToCreate);
			numToCreate = 50;
			numFailed += numToCreate - SpawnRandomMediumAsteroids(numToCreate);

			if(numFailed > 0)
				d2LogInfo << "Game::LoadLevel0: Entity creation attempt limit reached " << numFailed << " times";
		}

		ValidateWorldDimensions();
	}

	//+-----------------\-----------------------------------------
	//|	   ClearLevel   |
	//\-----------------/-----------------------------------------
	void Game::ClearLevel(const b2Vec2& newWorldDimensions)
	{
		m_camera.Init(m_settings.cameraDimensionRange, m_settings.cameraZoomSpeed, m_settings.cameraInitialZoomOutPercent);
		m_cameraFollowingEntity = false;
		m_starfield.Init(m_settings.starfield);
		m_playerSet = false;
		m_delayedLevelChange = false;

		d2d::Rect worldRect;
		worldRect.SetCenter(b2Vec2_zero, newWorldDimensions);
		m_world.Init(worldRect);
	}

	//+-----------------\-----------------------------------------
	//|	  SpawnPlayer   |
	//\-----------------/-----------------------------------------
	void Game::SpawnPlayer()
	{
		// Player
		InstanceDef def{ .position{ b2Vec2_zero }, .angle{ d2d::PI_OVER_TWO } };
		CreatePlayer(m_world, def);
	}

	//+-----------------\-----------------------------------------
	//|	   SpawnExit    |
	//\-----------------/-----------------------------------------
	void Game::SpawnExit()
	{
		InstanceDef def{ .position{ -5.0f, -12.0f } };
		CreateExit(m_world, def);
	}

	//+--------------------------------\--------------------------
	//|	      SpawnRandomIcons         |
	//\--------------------------------/--------------------------
	// returns number of entities created
	unsigned Game::SpawnRandomIcons(unsigned count)
	{
		const float minGap = MIN_BOUNDING_RADII_GAP_RELATIVE_TO_HEIGHT_ICONS * ICON_HEIGHT;
		unsigned numCreated = 0;
		for(unsigned i = 0; i < count; ++i)
		{
			int model{ d2d::RandomInt({0, NUM_ICON_MODELS - 1}) };
			b2Vec2 size;
			size.y = ICON_HEIGHT;
			size.x = size.y * m_models.textures.icons[model].GetWidthToHeightRatio();
			float boundingRadius = size.Length() * 0.5f;

			InstanceDef def;
			bool positionFound =
					m_world.GetRandomPositionAwayFromExistingEntities(
						boundingRadius, minGap, MAX_ATTEMPTS_PER_ENTITY, def.position);
			if(positionFound)
			{
				def.angle = 0.0f;
				CreateIcon(m_world, model, def);
				numCreated++;
			}
		}
		return numCreated;
	}

	//+--------------------------------\--------------------------
	//|	  SpawnRandomXLargeAsteroids   |
	//\--------------------------------/--------------------------
	// returns number of entities created
	unsigned Game::SpawnRandomXLargeAsteroids(unsigned count)
	{
		unsigned numCreated = 0;
		const float minGap = XLARGE_ASTEROID_HEIGHT * MIN_BOUNDING_RADII_GAP_RELATIVE_TO_HEIGHT;
		for(unsigned i = 0; i < count; ++i)
		{
			int model = d2d::RandomInt({0, NUM_XLARGE_ASTEROID_MODELS - 1});
			b2Vec2 size;
			size.y = XLARGE_ASTEROID_HEIGHT * XLARGE_ASTEROID_RELATIVE_HEIGHTS[model];
			size.x = size.y * m_models.textures.asteroidsXLarge[model].GetWidthToHeightRatio();
			float boundingRadius = size.Length() * 0.5f;

			InstanceDef def;
			bool positionFound =
					m_world.GetRandomPositionAwayFromExistingEntities(
						boundingRadius, minGap, MAX_ATTEMPTS_PER_ENTITY, def.position);
			if(positionFound)
			{
				def.angle = d2d::RandomFloat({ 0.0f, d2d::TWO_PI });
				CreateXLargeAsteroid(m_world, model, d2d::RandomBool(), def);
				numCreated++;
			}
		}
		return numCreated;
	}

	//+--------------------------------\--------------------------
	//|	  SpawnRandomLargeAsteroids    |
	//\--------------------------------/--------------------------
	// returns number of entities created
	unsigned Game::SpawnRandomLargeAsteroids(unsigned count)
	{
		unsigned numCreated = 0;
		const float minGap = LARGE_ASTEROID_HEIGHT * MIN_BOUNDING_RADII_GAP_RELATIVE_TO_HEIGHT;
		for(unsigned i = 0; i < count; ++i)
		{
			int model = d2d::RandomInt({0, NUM_LARGE_ASTEROID_MODELS - 1});
			b2Vec2 size;
			size.y = LARGE_ASTEROID_HEIGHT * LARGE_ASTEROID_RELATIVE_HEIGHTS[model];
			size.x = size.y * m_models.textures.asteroidsLarge[model].GetWidthToHeightRatio();
			float boundingRadius = size.Length() * 0.5f;

			InstanceDef def;
			bool positionFound =
					m_world.GetRandomPositionAwayFromExistingEntities(
						boundingRadius, minGap, MAX_ATTEMPTS_PER_ENTITY, def.position);
			if(positionFound)
			{
				def.angle = d2d::RandomFloat({ 0.0f, d2d::TWO_PI });
				CreateLargeAsteroid(m_world, model, d2d::RandomBool(), def);
				numCreated++;
			}
		}
		return numCreated;
	}

	//+--------------------------------\--------------------------
	//|	  SpawnRandomMediumAsteroids   |
	//\--------------------------------/--------------------------
	// returns number of entities created
	unsigned Game::SpawnRandomMediumAsteroids(unsigned count)
	{
		unsigned numCreated = 0;
		const float minGap = MEDIUM_ASTEROID_HEIGHT * MIN_BOUNDING_RADII_GAP_RELATIVE_TO_HEIGHT;
		for(unsigned i = 0; i < count; ++i)
		{
			int model = d2d::RandomInt({0, NUM_MEDIUM_ASTEROID_MODELS - 1});
			b2Vec2 size;
			size.y = MEDIUM_ASTEROID_HEIGHT * MEDIUM_ASTEROID_RELATIVE_HEIGHTS[model];
			size.x = size.y * m_models.textures.asteroidsMedium[model].GetWidthToHeightRatio();
			float boundingRadius = size.Length() * 0.5f;

			InstanceDef def;
			bool positionFound =
					m_world.GetRandomPositionAwayFromExistingEntities(
						boundingRadius, minGap, MAX_ATTEMPTS_PER_ENTITY, def.position);
			if(positionFound)
			{
				def.angle = d2d::RandomFloat({ 0.0f, d2d::TWO_PI });
				CreateMediumAsteroid(m_world, model, d2d::RandomBool(), def);
				numCreated++;
			}
		}
		return numCreated;
	}

	//+--------------------------------\--------------------------
	//|	  SpawnRandomSmallAsteroids    |
	//\--------------------------------/--------------------------
	// returns number of entities created
	unsigned Game::SpawnRandomSmallAsteroids(unsigned count)
	{
		unsigned numCreated = 0;
		const float minGap = SMALL_ASTEROID_HEIGHT * MIN_BOUNDING_RADII_GAP_RELATIVE_TO_HEIGHT;
		for(unsigned i = 0; i < count; ++i)
		{
			int model = d2d::RandomInt({0, NUM_SMALL_ASTEROID_MODELS - 1});
			b2Vec2 size;
			size.y = SMALL_ASTEROID_HEIGHT * SMALL_ASTEROID_RELATIVE_HEIGHTS[model];
			size.x = size.y * m_models.textures.asteroidsSmall[model].GetWidthToHeightRatio();
			float boundingRadius = size.Length() * 0.5f;

			InstanceDef def;
			bool positionFound =
					m_world.GetRandomPositionAwayFromExistingEntities(
						boundingRadius, minGap, MAX_ATTEMPTS_PER_ENTITY, def.position);
			if(positionFound)
			{
				def.angle = d2d::RandomFloat({ 0.0f, d2d::TWO_PI });
				CreateSmallAsteroid(m_world, model, d2d::RandomBool(), def);
				numCreated++;
			}
		}
		return numCreated;
	}

	//+--------------------------------\--------------------------
	//|	   ValidateWorldDimensions     |
	//\--------------------------------/--------------------------
	// Make sure world is not too small
	void Game::ValidateWorldDimensions() const
	{
		// Limit world dimensions relative to maximum camera dimensions
		float width = m_world.GetWorldRect().GetWidth();
		float height = m_world.GetWorldRect().GetHeight();
		float max = m_camera.GetDimensionRange().GetMax();
		if(width < max * MIN_WORLD_TO_CAMERA_RATIO)
			throw GameException{ "World width(" + d2d::ToString(width) + ") too small compared to max camera width(" + d2d::ToString(max) };
		if(height < max * MIN_WORLD_TO_CAMERA_RATIO)
			throw GameException{ "World height(" + d2d::ToString(height) + ") too small compared to max camera height(" + d2d::ToString(max) };
	}
	void Game::SetPlayer(WorldID entityID)
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
	void Game::FollowEntity(WorldID entityID)
	{
		m_cameraFollowingEntity = true;
		m_cameraFollowEntityID = entityID;
	}
	WorldID Game::CreateExit(World& world, const InstanceDef& def)
	{
		InstanceDef tempDef;
		tempDef.position = def.position;
		tempDef.velocity = def.velocity;

		tempDef.position.x -= 10.0f;
		tempDef.angle = 0.0f;
		CreateBumper(world, tempDef);

		tempDef.position.x += 20.0f;
		tempDef.angle = d2d::PI;
		CreateBumper(world, tempDef);

		tempDef.position = def.position;
		tempDef.angle = 0.0f;
		WorldID id3 = CreateExitSensor(world, tempDef);

		return id3;
	}
	//+---------------------------\-------------------------------
	//|	  Level Exit Components   |
	//\---------------------------/-------------------------------
	WorldID Game::CreateExitSensor(World& world, const InstanceDef& def)
	{
		//d2Assert(entityID < WORLD_MAX_ENTITIES);
		//d2Assert(bumperID1 < WORLD_MAX_ENTITIES);
		//d2Assert(bumperID2 < WORLD_MAX_ENTITIES);
		//b2Vec2 sensorSize;
		//b2Transform transform1 = GetSmoothedTransform(bumperID1);
		//b2Transform transform2 = GetSmoothedTransform(bumperID2);
		//b2Vec2 position1 = transform1.p;
		//b2Vec2 position2 = transform2.p;
		//b2Vec2 midPoint = 0.5f * (position1 + position2);
		//float height = 2.0f;
		//float width = (position2 - position1).Length();

		//b2BodyDef bodyDef;
		//bodyDef.type = b2_dynamicBody;
		//bodyDef.position = midPoint;
		//b2Body* exitBodyPtr = m_b2WorldPtr->CreateBody(&bodyDef);

		//b2PolygonShape exitShape;
		//exitShape.SetAsBox(width, height);
		//b2Fixture* exitFixturePtr = exitBodyPtr->CreateFixture(&exitShape, 1.0f);



		//m_componentBits[entityID] |= COMPONENT_EXIT;
		//m_exitComponents[entityID].bumperID1 = bumperID1;
		//m_exitComponents[entityID].bumperID2 = bumperID2;
		//m_exitComponents[entityID].bodyPtr = exitBodyPtr;
		//m_exitComponents[entityID].fixturePtr = exitFixturePtr;
			//world.ApplyPrismaticJoint(id1, id2, { 1.0f, 0.0f }, b2Vec2_zero, b2Vec2_zero, d2d::PI, true, { 4.0f, 14.0f });
		b2Vec2 size = { 15.0f, 1.5f };
		WorldID id = world.NewEntityID(size, 0, def.activate);
		//def.angle = 0.0f;
		world.AddPhysicsComponent(id, b2_kinematicBody, def);
		world.AddRectShape(id, BUMPER_MATERIAL, BUMPER_FILTER, { 1.0f, 1.0f }, true);
		DrawFixturesComponent drawFixtures;
		drawFixtures.color = d2d::Color{ 0.0f, 1.0f, 0.0f, 0.6f };
		drawFixtures.fill = true;
		world.AddDrawFixturesComponent(id, drawFixtures);
		world.SetFlags(id, FLAG_EXIT, true);

		return id;
	}

	//+-------------\---------------------------------------------
	//|	 Entities   |
	//\-------------/---------------------------------------------
	WorldID Game::CreateBasicObject(World& world, const b2Vec2& size, int drawLayer,
		const Model& model, const d2d::Material& material, const d2d::Filter& filter,
		b2BodyType physicsType, const InstanceDef& def)
	{
		WorldID id = m_world.NewEntityID(size, drawLayer, def.activate);
		world.AddPhysicsComponent(id, physicsType, def);
		world.AddShapes(id, model.name, material, filter);
		world.AddDrawAnimationComponent(id, model.animationDef);
		return id;
	}
	void Game::CreatePlayer(World& world, const InstanceDef& def)
	{
		WorldID blasterID = CreateBlaster(world, def);
		world.AddIconCollectorComponent(blasterID);
		SetPlayer(blasterID);
		FollowEntity(blasterID);
	}
	WorldID Game::CreateScout(World& world, const InstanceDef& def)
	{
		b2Vec2 size{ SCOUT_HEIGHT * m_models.textures.scout.GetWidthToHeightRatio(), SCOUT_HEIGHT };
		WorldID id = CreateBasicObject(world, size, DEFAULT_DRAW_LAYER, m_models.scout, SHIP_MATERIAL, SHIP_FILTER, b2_dynamicBody, def);
		world.AddRotatorComponent(id, SCOUT_ROTATION_SPEED);

		world.AddThrusterComponent(id, 2);
		world.AddThruster(id, 0, m_models.scoutThruster.animationDef, SCOUT_THRUSTER_ACCELERATION, SCOUT_THRUSTER_FUEL_PER_SECOND, { SCOUT_THRUSTER_OFFSET_X,  SCOUT_THRUSTER_SPREAD_Y });
		world.AddThruster(id, 1, m_models.scoutThruster.animationDef, SCOUT_THRUSTER_ACCELERATION, SCOUT_THRUSTER_FUEL_PER_SECOND, { SCOUT_THRUSTER_OFFSET_X, -SCOUT_THRUSTER_SPREAD_Y });

		world.AddFuelComponent(id, SCOUT_MAX_FUEL, SCOUT_MAX_FUEL);
		world.AddBoosterComponent(id, SCOUT_BOOST_FACTOR, BOOST_SECONDS, BOOST_COOLDOWN_SECONDS);

		world.AddBrakeComponent(id, SCOUT_BRAKE_DECELERATION);

		// Bullets
		world.AddProjectileLauncherComponent(id, 1, false);
		world.AddProjectileLauncher(id, 0, m_bulletDef, { SCOUT_PROJECTILE_OFFSET_X, 0.0f }, SCOUT_CANON_IMPULSE, SCOUT_CANON_INTERVAL, false, false);
		//world.AddProjectileLauncher(id, 1, m_bulletDef, { SCOUT_PROJECTILE_OFFSET_X,  SCOUT_PROJECTILE_SPREAD_Y }, SCOUT_CANON_IMPULSE, SCOUT_CANON_INTERVAL, false, false);
		//world.AddProjectileLauncher(id, 2, m_bulletDef, { SCOUT_PROJECTILE_OFFSET_X, -SCOUT_PROJECTILE_SPREAD_Y }, SCOUT_CANON_IMPULSE, SCOUT_CANON_INTERVAL, false, false);

		world.AddHealthComponent(id, SCOUT_HP);
		world.AddParticleExplosionOnDeathComponent(id, PARTICLE_EXPLOSION_RELATIVE_SIZE,
			SCOUT_NUM_PARTICLES, SCOUT_PARTICLE_SPEED_RANGE, DAMAGE_BASED_SPEED_INCREASE_FACTOR,
			SCOUT_PARTICLE_SIZE_INDEX_RANGE, BLASTER_PARTICLE_COLOR_RANGE,
			SCOUT_PARTICLE_LIFETIME, PARTICLE_EXPLOSION_FADEIN, SCOUT_PARTICLE_FADEOUT);
		return id;
	}
	WorldID Game::CreateBlaster(World& world, const InstanceDef& def)
	{
		b2Vec2 size{ BLASTER_HEIGHT * m_models.textures.blaster.GetWidthToHeightRatio(), BLASTER_HEIGHT };
		WorldID id = CreateBasicObject(world, size, DEFAULT_DRAW_LAYER, m_models.blaster, SHIP_MATERIAL, SHIP_FILTER, b2_dynamicBody, def);
		world.AddRotatorComponent(id, BLASTER_ROTATION_SPEED);

		world.AddThrusterComponent(id, 4);
		world.AddThruster(id, 0, m_models.blasterThruster.animationDef, BLASTER_THRUSTER_ACCELERATION, BLASTER_THRUSTER_FUEL_PER_SECOND, { BLASTER_THRUSTER_OFFSET_X,  BLASTER_THRUSTER_INNER_SPREAD_Y });
		world.AddThruster(id, 1, m_models.blasterThruster.animationDef, BLASTER_THRUSTER_ACCELERATION, BLASTER_THRUSTER_FUEL_PER_SECOND, { BLASTER_THRUSTER_OFFSET_X, -BLASTER_THRUSTER_INNER_SPREAD_Y });
		world.AddThruster(id, 2, m_models.blasterThruster.animationDef, BLASTER_THRUSTER_ACCELERATION, BLASTER_THRUSTER_FUEL_PER_SECOND, { BLASTER_THRUSTER_OFFSET_X,  BLASTER_THRUSTER_OUTER_SPREAD_Y });
		world.AddThruster(id, 3, m_models.blasterThruster.animationDef, BLASTER_THRUSTER_ACCELERATION, BLASTER_THRUSTER_FUEL_PER_SECOND, { BLASTER_THRUSTER_OFFSET_X, -BLASTER_THRUSTER_OUTER_SPREAD_Y });

		world.AddFuelComponent(id, BLASTER_MAX_FUEL, BLASTER_MAX_FUEL);
		world.AddBoosterComponent(id, BLASTER_BOOST_FACTOR, BOOST_SECONDS, BOOST_COOLDOWN_SECONDS);

		world.AddBrakeComponent(id, BLASTER_BRAKE_DECELERATION);

		// Bullets
		world.AddProjectileLauncherComponent(id, 5, false);
		world.AddProjectileLauncher(id, 0, m_bulletDef, { BLASTER_PROJECTILE_OFFSET_X, 0.0f }, BLASTER_CANON_IMPULSE, BLASTER_CANON_INTERVAL, false, false);
		//world.AddProjectileLauncher(id, 1, m_bulletDef, { BLASTER_PROJECTILE_OFFSET_X,  BLASTER_PROJECTILE_INNER_SPREAD_Y }, BLASTER_CANON_IMPULSE, BLASTER_CANON_INTERVAL, false, false);
		//world.AddProjectileLauncher(id, 2, m_bulletDef, { BLASTER_PROJECTILE_OFFSET_X, -BLASTER_PROJECTILE_INNER_SPREAD_Y }, BLASTER_CANON_IMPULSE, BLASTER_CANON_INTERVAL, false, false);
		//world.AddProjectileLauncher(id, 3, m_bulletDef, { BLASTER_PROJECTILE_OFFSET_X,  BLASTER_PROJECTILE_OUTER_SPREAD_Y }, BLASTER_CANON_IMPULSE, BLASTER_CANON_INTERVAL, false, false);
		//world.AddProjectileLauncher(id, 4, m_bulletDef, { BLASTER_PROJECTILE_OFFSET_X, -BLASTER_PROJECTILE_OUTER_SPREAD_Y }, BLASTER_CANON_IMPULSE, BLASTER_CANON_INTERVAL, false, false);

		// Missiles
		//world.AddProjectileLauncherComponent(id, 5, true);
		//world.AddProjectileLauncher(id, 0, m_fatMissileDef, { BLASTER_PROJECTILE_OFFSET_X, 0.0f }, BLASTER_MISSILE_LAUNCHER_IMPULSE, BLASTER_MISSILE_LAUNCHER_INTERVAL, false, true);
		//world.AddProjectileLauncher(id, 1, m_missileDef, { BLASTER_PROJECTILE_OFFSET_X,  BLASTER_PROJECTILE_INNER_SPREAD_Y }, BLASTER_MISSILE_LAUNCHER_IMPULSE, BLASTER_MISSILE_LAUNCHER_INTERVAL, false, true);
		//world.AddProjectileLauncher(id, 2, m_missileDef, { BLASTER_PROJECTILE_OFFSET_X, -BLASTER_PROJECTILE_INNER_SPREAD_Y }, BLASTER_MISSILE_LAUNCHER_IMPULSE, BLASTER_MISSILE_LAUNCHER_INTERVAL, false, true);
		//world.AddProjectileLauncher(id, 3, m_missileDef, { BLASTER_PROJECTILE_OFFSET_X,  BLASTER_PROJECTILE_OUTER_SPREAD_Y }, BLASTER_MISSILE_LAUNCHER_IMPULSE, BLASTER_MISSILE_LAUNCHER_INTERVAL, false, true);
		//world.AddProjectileLauncher(id, 4, m_missileDef, { BLASTER_PROJECTILE_OFFSET_X, -BLASTER_PROJECTILE_OUTER_SPREAD_Y }, BLASTER_MISSILE_LAUNCHER_IMPULSE, BLASTER_MISSILE_LAUNCHER_INTERVAL, false, true);

		world.AddHealthComponent(id, BLASTER_HP);
		world.AddParticleExplosionOnDeathComponent(id, PARTICLE_EXPLOSION_RELATIVE_SIZE,
			BLASTER_NUM_PARTICLES, BLASTER_PARTICLE_SPEED_RANGE, DAMAGE_BASED_SPEED_INCREASE_FACTOR,
			BLASTER_PARTICLE_SIZE_INDEX_RANGE, BLASTER_PARTICLE_COLOR_RANGE,
			BLASTER_PARTICLE_LIFETIME, PARTICLE_EXPLOSION_FADEIN, BLASTER_PARTICLE_FADEOUT);
		return id;
	}
	WorldID Game::CreateXLargeAsteroid(World& world, unsigned modelIndex, bool isRock, const InstanceDef& def)
	{
		d2Assert(modelIndex < NUM_XLARGE_ASTEROID_MODELS);
		float height{ XLARGE_ASTEROID_HEIGHT * XLARGE_ASTEROID_RELATIVE_HEIGHTS[modelIndex] };
		b2Vec2 size{ height * m_models.textures.asteroidsXLarge[modelIndex].GetWidthToHeightRatio(), height };
		Model& model = isRock ? m_models.rocksXLarge.at(modelIndex) : m_models.asteroidsXLarge.at(modelIndex);
		WorldID id = CreateBasicObject(world, size, DEFAULT_DRAW_LAYER, model, ASTEROID_MATERIAL, ASTEROID_FILTER, b2_dynamicBody, def);
		world.AddHealthComponent(id, XLARGE_ASTEROID_HP);
		world.AddParticleExplosionOnDeathComponent(id, PARTICLE_EXPLOSION_RELATIVE_SIZE,
			XLARGE_ASTEROID_NUM_PARTICLES, ASTEROID_PARTICLE_SPEED_RANGE, DAMAGE_BASED_SPEED_INCREASE_FACTOR,
			XLARGE_ASTEROID_PARTICLE_SIZE_INDEX_RANGE, isRock ? ROCK_PARTICLE_COLOR_RANGE : ASTEROID_PARTICLE_COLOR_RANGE,
			XLARGE_ASTEROID_PARTICLE_LIFETIME, PARTICLE_EXPLOSION_FADEIN, XLARGE_ASTEROID_PARTICLE_FADEOUT);
		return id;
	}
	WorldID Game::CreateLargeAsteroid(World& world, unsigned modelIndex, bool isRock, const InstanceDef& def)
	{
		d2Assert(modelIndex < NUM_LARGE_ASTEROID_MODELS);
		float height{ LARGE_ASTEROID_HEIGHT * LARGE_ASTEROID_RELATIVE_HEIGHTS[modelIndex] };
		b2Vec2 size{ height * m_models.textures.asteroidsLarge[modelIndex].GetWidthToHeightRatio(), height };
		Model& model = isRock ? m_models.rocksLarge.at(modelIndex) : m_models.asteroidsLarge.at(modelIndex);
		WorldID id = CreateBasicObject(world, size, DEFAULT_DRAW_LAYER, model, ASTEROID_MATERIAL, ASTEROID_FILTER, b2_dynamicBody, def);
		world.AddHealthComponent(id, LARGE_ASTEROID_HP);
		world.AddParticleExplosionOnDeathComponent(id, PARTICLE_EXPLOSION_RELATIVE_SIZE,
			LARGE_ASTEROID_NUM_PARTICLES, ASTEROID_PARTICLE_SPEED_RANGE, DAMAGE_BASED_SPEED_INCREASE_FACTOR,
			LARGE_ASTEROID_PARTICLE_SIZE_INDEX_RANGE, isRock ? ROCK_PARTICLE_COLOR_RANGE : ASTEROID_PARTICLE_COLOR_RANGE,
			LARGE_ASTEROID_PARTICLE_LIFETIME, PARTICLE_EXPLOSION_FADEIN, LARGE_ASTEROID_PARTICLE_FADEOUT);
		return id;
	}
	WorldID Game::CreateMediumAsteroid(World& world, unsigned modelIndex, bool isRock, const InstanceDef& def)
	{
		d2Assert(modelIndex < NUM_MEDIUM_ASTEROID_MODELS);
		float height{ MEDIUM_ASTEROID_HEIGHT * MEDIUM_ASTEROID_RELATIVE_HEIGHTS[modelIndex] };
		b2Vec2 size{ height * m_models.textures.asteroidsMedium[modelIndex].GetWidthToHeightRatio(), height };
		Model& model = isRock ? m_models.rocksMedium.at(modelIndex) : m_models.asteroidsMedium.at(modelIndex);
		WorldID id = CreateBasicObject(world, size, DEFAULT_DRAW_LAYER, model, ASTEROID_MATERIAL, ASTEROID_FILTER, b2_dynamicBody, def);
		world.AddHealthComponent(id, MEDIUM_ASTEROID_HP);
		world.AddParticleExplosionOnDeathComponent(id, PARTICLE_EXPLOSION_RELATIVE_SIZE,
			MEDIUM_ASTEROID_NUM_PARTICLES, ASTEROID_PARTICLE_SPEED_RANGE, DAMAGE_BASED_SPEED_INCREASE_FACTOR,
			MEDIUM_ASTEROID_PARTICLE_SIZE_INDEX_RANGE, isRock ? ROCK_PARTICLE_COLOR_RANGE : ASTEROID_PARTICLE_COLOR_RANGE,
			MEDIUM_ASTEROID_PARTICLE_LIFETIME, PARTICLE_EXPLOSION_FADEIN, MEDIUM_ASTEROID_PARTICLE_FADEOUT);
		return id;
	}
	WorldID Game::CreateSmallAsteroid(World& world, unsigned modelIndex, bool isRock, const InstanceDef& def)
	{
		d2Assert(modelIndex < NUM_SMALL_ASTEROID_MODELS);
		float height{ SMALL_ASTEROID_HEIGHT * SMALL_ASTEROID_RELATIVE_HEIGHTS[modelIndex] };
		b2Vec2 size{ height * m_models.textures.asteroidsSmall[modelIndex].GetWidthToHeightRatio(), height };
		Model& model = isRock ? m_models.rocksSmall.at(modelIndex) : m_models.asteroidsSmall.at(modelIndex);
		WorldID id = CreateBasicObject(world, size, DEFAULT_DRAW_LAYER, model, ASTEROID_MATERIAL, ASTEROID_FILTER, b2_dynamicBody, def);
		world.AddHealthComponent(id, SMALL_ASTEROID_HP);
		world.AddParticleExplosionOnDeathComponent(id, PARTICLE_EXPLOSION_RELATIVE_SIZE,
			SMALL_ASTEROID_NUM_PARTICLES, ASTEROID_PARTICLE_SPEED_RANGE, DAMAGE_BASED_SPEED_INCREASE_FACTOR,
			SMALL_ASTEROID_PARTICLE_SIZE_INDEX_RANGE, isRock ? ROCK_PARTICLE_COLOR_RANGE : ASTEROID_PARTICLE_COLOR_RANGE,
			SMALL_ASTEROID_PARTICLE_LIFETIME, PARTICLE_EXPLOSION_FADEIN, SMALL_ASTEROID_PARTICLE_FADEOUT);
		return id;
	}
	WorldID Game::CreateBumper(World& world, const InstanceDef& def)
	{
		b2Vec2 size{ BUMPER_HEIGHT * m_models.textures.bumper.GetWidthToHeightRatio(), BUMPER_HEIGHT };
		WorldID id = CreateBasicObject(world, size, DEFAULT_DRAW_LAYER, m_models.bumper, BUMPER_MATERIAL, BUMPER_FILTER, b2_kinematicBody, def);
		//WorldID id = CreateBasicObject(world, size, DEFAULT_DRAW_LAYER, m_bumperModel, BUMPER_MATERIAL, BUMPER_FILTER, b2_dynamicBody, def);
		return id;
	}
	WorldID Game::CreateSoda(World& world, const InstanceDef& def)
	{
		b2Vec2 size{ SODA_HEIGHT * m_models.textures.soda.GetWidthToHeightRatio(), SODA_HEIGHT };
		WorldID id = CreateBasicObject(world, size, DEFAULT_DRAW_LAYER, m_models.soda, FUEL_MATERIAL, FUEL_FILTER, b2_dynamicBody, def);
		PowerUpComponent powerUp;
		powerUp.type = PowerUpType::FUEL;
		powerUp.value = 10;
		m_world.AddPowerUpComponent(id, powerUp);
		return id;
	}
	WorldID Game::CreateMelon(World& world, const InstanceDef& def)
	{
		b2Vec2 size{ MELON_HEIGHT * m_models.textures.melon.GetWidthToHeightRatio(), MELON_HEIGHT };
		WorldID id = CreateBasicObject(world, size, DEFAULT_DRAW_LAYER, m_models.melon, FUEL_MATERIAL, FUEL_FILTER, b2_dynamicBody, def);
		PowerUpComponent powerUp;
		powerUp.type = PowerUpType::FUEL;
		powerUp.value = 20;
		m_world.AddPowerUpComponent(id, powerUp);
		return id;
	}
	WorldID Game::CreateApple(World& world, const InstanceDef& def)
	{
		b2Vec2 size{ APPLE_HEIGHT * m_models.textures.apple.GetWidthToHeightRatio(), APPLE_HEIGHT };
		size = 4.0f * size;
		WorldID id = CreateBasicObject(world, size, DEFAULT_DRAW_LAYER, m_models.apple, FUEL_MATERIAL, FUEL_FILTER, b2_dynamicBody, def);
		PowerUpComponent powerUp;
		powerUp.type = PowerUpType::FUEL;
		powerUp.value = 5;
		m_world.AddPowerUpComponent(id, powerUp);
		return id;
	}
	WorldID Game::CreateIcon(World& world, unsigned modelIndex, const InstanceDef& def)
	{
		d2Assert(modelIndex < m_models.textures.icons.size());
		b2Vec2 size{ ICON_HEIGHT * m_models.textures.icons.at(0).GetWidthToHeightRatio(), ICON_HEIGHT };
		Model model{ "icon", { m_models.textures.icons.at(modelIndex) } };
		WorldID id = CreateBasicObject(world, size, DEFAULT_DRAW_LAYER, model, ICON_MATERIAL, ICON_FILTER, b2_dynamicBody, def);
		PowerUpComponent powerUp;
		powerUp.type = PowerUpType::ICON;
		powerUp.value = 1;
		m_world.AddPowerUpComponent(id, powerUp);
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
				m_delayedLevelChange = false;
				m_currentLevel++;
				StartCurrentLevel();
			}
		}

		// Update
		m_world.Update(dt, playerController);
		UpdateCamera(dt, playerController);
	}
	void Game::StartDelayedLevelChange(float delay)
	{
		m_delayedLevelChange = true;
		m_levelChangeDelayTime = delay;
		m_levelChangeDelayTimeAccumulator = 0.0f;
	}
	void Game::UpdateCamera(float dt, const PlayerController& playerController)
	{
		if(m_cameraFollowingEntity)
			if(m_world.HasPhysics(m_cameraFollowEntityID))
				m_camera.SetPosition(b2Mul(m_world.GetSmoothedTransform(m_cameraFollowEntityID),
					m_world.GetLocalCenterOfMass(m_cameraFollowEntityID)));
		m_camera.Update(dt, playerController.zoomOutFactor);
	}

	//+--------------------------\--------------------------------
	//|	 EntityWillBeDestroyed   | override (DestroyListener)
	//\--------------------------/--------------------------------
	void Game::EntityWillBeDestroyed(WorldID entityID)
	{
		if(m_cameraFollowingEntity && entityID == m_cameraFollowEntityID)
			m_cameraFollowingEntity = false;

		if(m_playerSet && entityID == m_playerID)
		{
			StartDelayedLevelChange(3.0f);
			m_playerSet = false;
		}
	}
	//+--------------------------\--------------------------------
	//|	      EntityWrapped      | override (WrapListener)
	//\--------------------------/--------------------------------
	void Game::EntityWrapped(WorldID entityID, const b2Vec2& translation)
	{
		if(m_cameraFollowingEntity && entityID == m_cameraFollowEntityID)
			m_starfield.Translate(translation);
	}
	//+--------------------------\--------------------------------
	//|	    LaunchProjectile     | override (ProjectileLauncherCallback)
	//\--------------------------/--------------------------------
	WorldID Game::LaunchProjectile(const ProjectileDef& projectileDef, const b2Vec2& position,
		float angle, float impulse, const b2Vec2& parentVelocity, WorldID parentID)
	{
		WorldID id{ m_world.NewEntityID(projectileDef.dimensions, m_world.GetDrawLayer(parentID) - 1, true) };
		InstanceDef def;
		def.position = position;
		def.angle = angle;
		def.velocity = parentVelocity;
		def.angularVelocity = 0.0f;
		m_world.AddPhysicsComponent(id, b2_dynamicBody, def,
			projectileDef.fixedRotation, projectileDef.continuousCollisionDetection);

		m_world.AddShapes(id, projectileDef.model.name, projectileDef.material, projectileDef.filter);

		if(impulse > 0.0f)
		{
			b2Vec2 unitDirectionVector{ cos(angle), sin(angle) };
			m_world.ApplyLinearImpulseToCenter(id, impulse * unitDirectionVector);
		}

		m_world.AddDrawAnimationComponent(id, projectileDef.model.animationDef);

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
			m_world.AddThruster(id, 0, {}, projectileDef.acceleration, 0.0f, b2Vec2_zero);
			m_world.AddSetThrustFactorAfterDelayComponent(id, 0.0f, projectileDef.accelerationTime);
		}
		return id;
	}
	//+-------------\---------------------------------------------
	//|	   Draw     |
	//\-------------/---------------------------------------------
	void Game::Draw()
	{
		b2Vec2 resolution{ d2d::Window::GetScreenResolution() };
		d2d::Rect cameraRect;
		cameraRect.SetCenter(m_camera.GetPosition(), m_camera.GetDimensions(d2d::Window::GetXYAspectRatio()));
		d2d::Window::SetCameraRect(cameraRect);

		m_starfield.Draw(m_camera.GetPosition());
		m_world.Draw();

		// Text draw mode
		d2d::Window::SetCameraRect({ b2Vec2_zero, resolution });
		d2d::Window::DisableTextures();
		d2d::Window::EnableBlending();

		if(m_playerSet)
		{
			// Draw fuel
			if(m_world.HasComponents(m_playerID, COMPONENT_FUEL))
			{
				d2d::Window::SetColor(m_fuelTextStyle.color);
				d2d::Window::PushMatrix();
				d2d::Window::Translate(m_fuelPosition * resolution);
				{
					int fuelInt = (int)(m_world.GetFuelLevel(m_playerID) + 0.5f);
					int maxFuelInt = (int)(m_world.GetMaxFuelLevel(m_playerID) + 0.5f);
					std::string fuelString = d2d::ToString(fuelInt) + "/" + d2d::ToString(maxFuelInt);
					d2d::Window::DrawString(fuelString, m_fuelAlignment, m_fuelTextStyle.size * resolution.y, m_fuelTextStyle.font);
				}
				d2d::Window::PopMatrix();
			}
			// Draw icons collected
			if(m_world.HasComponents(m_playerID, COMPONENT_ICON_COLLECTOR))
			{
				d2d::Window::SetColor(m_iconsTextStyle.color);
				d2d::Window::PushMatrix();
				d2d::Window::Translate(m_iconsPosition * resolution);
				{
					unsigned icons = m_world.GetIconsCollected(m_playerID);
					std::string iconsString = d2d::ToString(icons);
					d2d::Window::DrawString(iconsString, m_iconsAlignment, m_iconsTextStyle.size * resolution.y, m_iconsTextStyle.font);
				}
				d2d::Window::PopMatrix();
			}
		}

	}
}
