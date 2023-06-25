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
	Game::Game(Camera* cameraPtr, Starfield* starfieldPtr)
		: m_cameraPtr{ cameraPtr }, m_starfieldPtr{ starfieldPtr }
	{
		d2Assert(m_cameraPtr);
		d2Assert(m_starfieldPtr);

		m_world.SetDestructionListener(this);
		m_world.SetWrapListener(this);
		m_world.SetProjectileLauncherListener(this);
		m_world.SetExitListener(this);
	}

	//+-----------------\-----------------------------------------
	//|	    NewGame     |
	//\-----------------/-----------------------------------------
	void Game::NewGame()
	{
		m_player.credits = 0.0f;
		m_player.currentLevel = 1;
		m_player.upgrades.clear();
	}

	//+-----------------\-----------------------------------------
	//|	   ClearLevel   |
	//\-----------------/-----------------------------------------
	void Game::ClearLevel(const b2Vec2& newWorldDimensions)
	{
		m_starfieldPtr->Randomize();
		m_cameraPtr->ResetZoom();
		m_cameraFollowingEntity = false;
		m_player.isSet = false;
		m_player.exited = false;
		m_delayedGameActions.clear();
		m_firstUpdate = true;

		d2d::Rect worldRect;
		worldRect.SetCenter(b2Vec2_zero, newWorldDimensions);
		m_world.Init(worldRect);
	}

	//+--------------------------------\--------------------------
	//|	      StartCurrentLevel        |
	//\--------------------------------/--------------------------
	void Game::StartCurrentLevel()
	{
		ClearLevel({ 500.0f, 500.0f });
		SpawnPlayer();
		SpawnExit();

		unsigned numFailed{ 0 };
		{
			unsigned numToCreate = 8;
			numFailed += numToCreate - SpawnRandomIcons(numToCreate);

			float angle = d2d::RandomFloat({ 0.0f, d2d::TWO_PI });
			m_spawnAsteroidStartingDirection = d2d::GetUnitVec2FromAngle(angle);

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

	//+-----------------------\-----------------------------------
	//|	  GetPlayerCredits    |
	//\-----------------------/-----------------------------------
	float Game::GetPlayerCredits() const
	{
		return m_player.credits;
	}

	//+-----------------------\-----------------------------------
	//|	   PurchaseUpgrade    |
	//\-----------------------/
	//	Returns true if purchase was successful,
	//	otherwise returns false.
	//------------------------------------------------------------
	bool Game::PurchaseUpgrade(ShopItemID itemID, float price)
	{
		if(m_player.upgrades.contains(itemID))
			return true;

		if(m_player.credits >= price)
		{
			m_player.upgrades.insert(itemID);
			m_player.credits -= price;
			if(m_player.isSet)
				ApplyPlayerUpgrades(m_world, m_player.id, m_player.upgrades);
			return true;
		}
		return false;
	}

	//+-----------------------\-----------------------------------
	//|	 ApplyPlayerUpgrades  |
	//\-----------------------/-----------------------------------
	void Game::ApplyPlayerUpgrades(World& world, WorldID playerID, const std::set<ShopItemID>& upgrades)
	{
		for(auto itemID : upgrades)
			switch(itemID)
			{
			case ShopItemID::GUNS_2:
				AddBlasterGuns(m_world, m_player.id, 2);
				break;
			case ShopItemID::GUNS_3:
				AddBlasterGuns(m_world, m_player.id, 3);
				break;
			case ShopItemID::GUNS_4:
				AddBlasterGuns(m_world, m_player.id, 4);
				break;
			case ShopItemID::GUNS_5:
				AddBlasterGuns(m_world, m_player.id, 5);
				break;
			}
	}

	//+-----------------\-----------------------------------------
	//|	 DidPlayerExit  |
	//\-----------------/-----------------------------------------
	bool Game::DidPlayerExit() const
	{
		return m_player.exited;
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
			int model = d2d::RandomInt({ 0, NUM_XLARGE_ASTEROID_MODELS - 1 });
			b2Vec2 size;
			size.y = XLARGE_ASTEROID_HEIGHT * XLARGE_ASTEROID_RELATIVE_HEIGHTS[model];
			size.x = size.y * m_models.textures.asteroidsXLarge[model].GetWidthToHeightRatio();
			float boundingRadius = size.Length() * 0.5f;

			float speed = d2d::RandomFloat(ASTEROID_STARTING_SPEED_RANGE_XL);
			float angularVel = d2d::RandomFloat(ASTEROID_STARTING_ANG_VEL_RANGE_XL);
			InstanceDef def{
				.velocity{speed* m_spawnAsteroidStartingDirection},
				.angularVelocity{angularVel} };
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
			int model = d2d::RandomInt({ 0, NUM_LARGE_ASTEROID_MODELS - 1 });
			b2Vec2 size;
			size.y = LARGE_ASTEROID_HEIGHT * LARGE_ASTEROID_RELATIVE_HEIGHTS[model];
			size.x = size.y * m_models.textures.asteroidsLarge[model].GetWidthToHeightRatio();
			float boundingRadius = size.Length() * 0.5f;

			float speed = d2d::RandomFloat(ASTEROID_STARTING_SPEED_RANGE_L);
			float angularVel = d2d::RandomFloat(ASTEROID_STARTING_ANG_VEL_RANGE_L);
			InstanceDef def{
				.velocity{speed* m_spawnAsteroidStartingDirection},
				.angularVelocity{angularVel} };
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
			int model = d2d::RandomInt({ 0, NUM_MEDIUM_ASTEROID_MODELS - 1 });
			b2Vec2 size;
			size.y = MEDIUM_ASTEROID_HEIGHT * MEDIUM_ASTEROID_RELATIVE_HEIGHTS[model];
			size.x = size.y * m_models.textures.asteroidsMedium[model].GetWidthToHeightRatio();
			float boundingRadius = size.Length() * 0.5f;

			float speed = d2d::RandomFloat(ASTEROID_STARTING_SPEED_RANGE_M);
			float angularVel = d2d::RandomFloat(ASTEROID_STARTING_ANG_VEL_RANGE_M);
			InstanceDef def{
				.velocity{speed* m_spawnAsteroidStartingDirection},
				.angularVelocity{angularVel} };
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
			int model = d2d::RandomInt({ 0, NUM_SMALL_ASTEROID_MODELS - 1 });
			b2Vec2 size;
			size.y = SMALL_ASTEROID_HEIGHT * SMALL_ASTEROID_RELATIVE_HEIGHTS[model];
			size.x = size.y * m_models.textures.asteroidsSmall[model].GetWidthToHeightRatio();
			float boundingRadius = size.Length() * 0.5f;

			float speed = d2d::RandomFloat(ASTEROID_STARTING_SPEED_RANGE_S);
			float angularVel = d2d::RandomFloat(ASTEROID_STARTING_ANG_VEL_RANGE_S);
			InstanceDef def{
				.velocity{speed * m_spawnAsteroidStartingDirection},
				.angularVelocity{angularVel} };
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
		float max = m_cameraPtr->GetDimensionRange().GetMax();
		if(width < max * MIN_WORLD_TO_CAMERA_RATIO)
			throw GameException{ "World width(" + d2d::ToString(width) + ") too small compared to max camera width(" + d2d::ToString(max) };
		if(height < max * MIN_WORLD_TO_CAMERA_RATIO)
			throw GameException{ "World height(" + d2d::ToString(height) + ") too small compared to max camera height(" + d2d::ToString(max) };
	}

	//+---------------------------\-------------------------------
	//|	        SetPlayer		  |
	//\---------------------------/-------------------------------
	void Game::SetPlayer(WorldID entityID)
	{
		if(m_world.EntityExists(entityID))
		{
			// Remove player controller from existing player entity
			if(m_player.isSet)
				m_world.SetFlags(m_player.id, FLAG_PLAYER_CONTROLLED, false);

			// Set new player
			m_world.SetFlags(entityID, FLAG_PLAYER_CONTROLLED, true);
			m_player.id = entityID;
			m_player.isSet = true;
		}
	}

	//+---------------------------\-------------------------------
	//|	        IsPlayer		  |
	//\---------------------------/-------------------------------
	bool Game::IsPlayer(WorldID entityID) const
	{
		return (m_player.isSet && entityID == m_player.id);
	}

	//+---------------------------\-------------------------------
	//|	      FollowEntity		  |
	//\---------------------------/-------------------------------
	void Game::FollowEntity(WorldID entityID)
	{
		m_cameraFollowingEntity = true;
		m_cameraFollowEntityID = entityID;
	}

	//+---------------------------\-------------------------------
	//|	      FollowEntity		  |
	//\---------------------------/-------------------------------
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
	//|	    CreateExitSensor	  |
	//\---------------------------/-------------------------------
	WorldID Game::CreateExitSensor(World& world, const InstanceDef& def)
	{
		b2Vec2 size = { 15.0f, 1.5f };
		WorldID id = world.NewEntityID(size, 0, def.activate);
		world.AddPhysicsComponent(id, b2_kinematicBody, def);
		world.AddRectShape(id, BUMPER_MATERIAL, BUMPER_FILTER, { 1.0f, 1.0f }, true);
		DrawFixturesComponent drawFixtures;
		drawFixtures.color = d2d::Color{ 0.0f, 1.0f, 0.0f, 0.6f };
		drawFixtures.fill = true;
		world.AddDrawFixturesComponent(id, drawFixtures);
		world.SetFlags(id, FLAG_EXIT, true);
		return id;
	}

	//+---------------------------\-------------------------------
	//|		CreateBasicObject	  |
	//\---------------------------/-------------------------------
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

	//+---------------------------\-------------------------------
	//|		 CreatePlayer		  |
	//\---------------------------/-------------------------------
	void Game::CreatePlayer(World& world, const InstanceDef& def)
	{
		WorldID blasterID = CreateBlaster(world, def);
		world.AddIconCollectorComponent(blasterID);
		SetPlayer(blasterID);
		ApplyPlayerUpgrades(world, blasterID, m_player.upgrades);
		FollowEntity(blasterID);
	}

	//+---------------------------\-------------------------------
	//|		  CreateScout		  |
	//\---------------------------/-------------------------------
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

	//+---------------------------\-------------------------------
	//|		  CreateBlaster		  |
	//\---------------------------/-------------------------------
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
		AddBlasterGuns(world, id, 1);

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

	//+---------------------------\-------------------------------
	//|		 AddBlasterGuns		  |
	//\---------------------------/-------------------------------
	void Game::AddBlasterGuns(World& world, WorldID entityID, unsigned numGuns)
	{
		d2d::Clamp(numGuns, { 1, 5 });
		world.AddProjectileLauncherComponent(entityID, numGuns, false);
		unsigned launcherSlot = 0;
		if(numGuns == 1 || numGuns == 3 || numGuns == 5)
			world.AddProjectileLauncher(entityID, launcherSlot++, m_bulletDef, { BLASTER_PROJECTILE_OFFSET_X, 0.0f }, BLASTER_CANON_IMPULSE, BLASTER_CANON_INTERVAL, false, false);
		if(numGuns >= 2)
		{
			world.AddProjectileLauncher(entityID, launcherSlot++, m_bulletDef, { BLASTER_PROJECTILE_OFFSET_X,  BLASTER_PROJECTILE_INNER_SPREAD_Y }, BLASTER_CANON_IMPULSE, BLASTER_CANON_INTERVAL, false, false);
			world.AddProjectileLauncher(entityID, launcherSlot++, m_bulletDef, { BLASTER_PROJECTILE_OFFSET_X, -BLASTER_PROJECTILE_INNER_SPREAD_Y }, BLASTER_CANON_IMPULSE, BLASTER_CANON_INTERVAL, false, false);
		}
		if(numGuns >= 4)
		{
			world.AddProjectileLauncher(entityID, launcherSlot++, m_bulletDef, { BLASTER_PROJECTILE_OFFSET_X,  BLASTER_PROJECTILE_OUTER_SPREAD_Y }, BLASTER_CANON_IMPULSE, BLASTER_CANON_INTERVAL, false, false);
			world.AddProjectileLauncher(entityID, launcherSlot++, m_bulletDef, { BLASTER_PROJECTILE_OFFSET_X, -BLASTER_PROJECTILE_OUTER_SPREAD_Y }, BLASTER_CANON_IMPULSE, BLASTER_CANON_INTERVAL, false, false);
		}
	}
	//+---------------------------\-------------------------------
	//|	   CreateXLargeAsteroid	  |
	//\---------------------------/-------------------------------
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

	//+---------------------------\-------------------------------
	//|	   CreateLargeAsteroid	  |
	//\---------------------------/-------------------------------
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

	//+---------------------------\-------------------------------
	//|	  CreateMediumAsteroid	  |
	//\---------------------------/-------------------------------
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

	//+---------------------------\-------------------------------
	//|	   CreateSmallAsteroid	  |
	//\---------------------------/-------------------------------
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

	//+---------------------------\-------------------------------
	//|	     CreateBumper		  |
	//\---------------------------/-------------------------------
	WorldID Game::CreateBumper(World& world, const InstanceDef& def)
	{
		b2Vec2 size{ BUMPER_HEIGHT * m_models.textures.bumper.GetWidthToHeightRatio(), BUMPER_HEIGHT };
		WorldID id = CreateBasicObject(world, size, DEFAULT_DRAW_LAYER, m_models.bumper, BUMPER_MATERIAL, BUMPER_FILTER, b2_kinematicBody, def);
		return id;
	}

	//+---------------------------\-------------------------------
	//|	       CreateSoda		  |
	//\---------------------------/-------------------------------
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

	//+---------------------------\-------------------------------
	//|		   CreateMelon		  |
	//\---------------------------/-------------------------------
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

	//+---------------------------\-------------------------------
	//|		   CreateApple		  |
	//\---------------------------/-------------------------------
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

	//+---------------------------\-------------------------------
	//|		   CreateIcon		  |
	//\---------------------------/-------------------------------
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
		m_world.Update(dt, playerController);
		b2Vec2 lastCameraPosition = m_cameraPtr->GetPosition();
		UpdateCamera(dt, playerController);
		if(m_firstUpdate)
		{
			m_starfieldPtr->MoveCameraWithoutMovingStars(m_cameraPtr->GetPosition() - lastCameraPosition);
			m_firstUpdate = false;
		}
		m_starfieldPtr->Update(m_cameraPtr->GetPosition());
		UpdateDelayedActions(dt);
	}

	//+--------------------------\--------------------------------
	//|	  UpdateDelayedActions   | 
	//\--------------------------/--------------------------------
	void Game::UpdateDelayedActions(float dt)
	{
		for(DelayedGameAction d : m_delayedGameActions)
			d.timeElapsed += dt;

		bool startLevel = false;
		for(auto it = m_delayedGameActions.begin(); it != m_delayedGameActions.end(); it++)
		{
			it->timeElapsed += dt;
			if(it->timeElapsed >= it->delay)
			{
				switch(it->action)
				{
				case GameAction::NEXT_LEVEL:
					m_player.currentLevel++;
					startLevel = true;
					break;
				case GameAction::RESTART_LEVEL:
					startLevel = true;
					break;
				}
				it = m_delayedGameActions.erase(it);
			}
		}
		if(startLevel)
		{
			StartCurrentLevel();
		}
	}

	//+--------------------------\--------------------------------
	//|	      UpdateCamera		 | 
	//\--------------------------/--------------------------------
	void Game::UpdateCamera(float dt, const PlayerController& playerController)
	{
		if(m_cameraFollowingEntity)
			if(m_world.HasPhysics(m_cameraFollowEntityID))
				m_cameraPtr->SetPosition(b2Mul(m_world.GetSmoothedTransform(m_cameraFollowEntityID),
					m_world.GetLocalCenterOfMass(m_cameraFollowEntityID)));
		m_cameraPtr->Update(dt, playerController.zoomOutFactor);
	}

	//+--------------------------\--------------------------------
	//|	 EntityWillBeDestroyed   | override (DestroyListener)
	//\--------------------------/--------------------------------
	void Game::EntityWillBeDestroyed(WorldID entityID)
	{
		if(m_cameraFollowingEntity && entityID == m_cameraFollowEntityID)
			m_cameraFollowingEntity = false;

		if(IsPlayer(entityID))
		{
			if(!m_player.exited)
			{
				m_player.credits -= DEATH_PENALTY_CREDITS;
				//m_delayedGameActions.push_back({ .action{ GameAction::RESTART_LEVEL }, .delay{ LEVEL_CHANGE_DELAY } });
			}
			m_player.isSet = false;
		}
	}
	//+--------------------------\--------------------------------
	//|	      EntityWrapped      | override (WrapListener)
	//\--------------------------/--------------------------------
	void Game::EntityWrapped(WorldID entityID, const b2Vec2& translation)
	{
		if(m_cameraFollowingEntity && entityID == m_cameraFollowEntityID)
			m_starfieldPtr->MoveCameraWithoutMovingStars(translation);
	}
	//+--------------------------\--------------------------------
	//|	   ProjectileLaunched    | override (ProjectileLauncherListener)
	//\--------------------------/--------------------------------
	void Game::ProjectileLaunched(const ProjectileDef& projectileDef, WorldID parentID)
	{}
	//+--------------------------\--------------------------------
	//|	      EntityExited       | override (ExitListener)
	//\--------------------------/--------------------------------
	void Game::EntityExited(WorldID entityID)
	{
		if(IsPlayer(entityID))
		{
			m_player.exited = true;
			if(m_world.HasComponents(entityID, COMPONENT_ICON_COLLECTOR))
				m_player.credits += (float)m_world.GetIconsCollected(entityID);
			m_player.currentLevel++;
		}
	}

	//+-------------\---------------------------------------------
	//|	   Draw     |
	//\-------------/---------------------------------------------
	void Game::Draw()
	{
		d2d::Window::SetCameraRect(m_cameraPtr->GetRect());
		m_starfieldPtr->Draw();
		m_world.Draw();
		DrawHUD();
	}

	//+-------------\---------------------------------------------
	//|	  DrawHUD   |
	//\-------------/---------------------------------------------
	void Game::DrawHUD()
	{
		// Text draw mode
		b2Vec2 resolution{ d2d::Window::GetScreenResolution() };
		d2d::Window::SetCameraRect({ b2Vec2_zero, resolution });
		d2d::Window::DisableTextures();
		d2d::Window::EnableBlending();

		if(m_player.isSet)
		{
			// Draw fuel
			if(m_world.HasComponents(m_player.id, COMPONENT_FUEL))
			{
				d2d::Window::SetColor(m_fuelTextStyle.color);
				d2d::Window::PushMatrix();
				d2d::Window::Translate(m_fuelPosition * resolution);
				{
					int fuelInt = (int)(m_world.GetFuelLevel(m_player.id) + 0.5f);
					int maxFuelInt = (int)(m_world.GetMaxFuelLevel(m_player.id) + 0.5f);
					std::string fuelString = d2d::ToString(fuelInt) + "/" + d2d::ToString(maxFuelInt);
					d2d::Window::DrawString(fuelString, m_fuelTextStyle.size * resolution.y, m_fuelTextStyle.fontRefPtr, m_fuelAlignment);
				}
				d2d::Window::PopMatrix();
			}

			// Draw icons collected
			if(m_world.HasComponents(m_player.id, COMPONENT_ICON_COLLECTOR))
			{
				d2d::Window::SetColor(m_iconsTextStyle.color);
				d2d::Window::PushMatrix();
				d2d::Window::Translate(m_iconsPosition * resolution);
				{
					unsigned icons = m_world.GetIconsCollected(m_player.id);
					std::string iconsString = d2d::ToString(icons);
					d2d::Window::DrawString(iconsString, m_iconsTextStyle.size * resolution.y, m_iconsTextStyle.fontRefPtr, m_iconsAlignment);
				}
				d2d::Window::PopMatrix();
			}

			// Draw credits
			d2d::Window::SetColor(m_creditsTextStyle.color);
			d2d::Window::PushMatrix();
			d2d::Window::Translate(m_creditsPosition * resolution);
			{
				d2d::Window::DrawString(d2d::ToString(m_player.credits), m_creditsTextStyle.size * resolution.y, m_creditsTextStyle.fontRefPtr, m_creditsAlignment);
			}
			d2d::Window::PopMatrix();

			// Draw level
			d2d::Window::SetColor(m_levelTextStyle.color);
			d2d::Window::PushMatrix();
			d2d::Window::Translate(m_levelPosition * resolution);
			{
				d2d::Window::DrawString(d2d::ToString(m_player.currentLevel), m_levelTextStyle.size * resolution.y, m_levelTextStyle.fontRefPtr, m_levelAlignment);
			}
			d2d::Window::PopMatrix();
		}
	}
}
