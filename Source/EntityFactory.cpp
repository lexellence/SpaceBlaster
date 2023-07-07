/**************************************************************************************\
** File: EntityFactory.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the EntityFactory class
**
\**************************************************************************************/
#include "pch.h"
#include "EntityFactory.h"
#include "GameSettings.h"
#include "Exceptions.h"
namespace Space
{
	//+---------------------------\-------------------------------
	//|		CreateBasicObject	  |
	//\---------------------------/-------------------------------
	WorldID EntityFactory::CreateBasicObject(World& world, const b2Vec2& size, int drawLayer,
		const Model& model, const d2d::Material& material, const d2d::Filter& filter,
		b2BodyType physicsType, const InstanceDef& def)
	{
		WorldID id = world.NewEntityID(size, drawLayer, def.activate);
		world.AddPhysicsComponent(id, physicsType, def);
		world.AddShapes(id, model.name, material, filter);
		world.AddDrawAnimationComponent(id, model.animationDef);
		return id;
	}

	//+---------------------------\-------------------------------
	//|		  CreateScout		  |
	//\---------------------------/-------------------------------
	WorldID EntityFactory::CreateScout(World& world, const InstanceDef& def)
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
		world.AddProjectileLauncher(id, 0, m_models.bulletDef, { SCOUT_PROJECTILE_OFFSET_X, 0.0f }, SCOUT_CANON_IMPULSE, SCOUT_CANON_INTERVAL, false, false);
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
	WorldID EntityFactory::CreateBlaster(World& world, const InstanceDef& def)
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
	void EntityFactory::AddBlasterGuns(World& world, WorldID entityID, unsigned numGuns)
	{
		d2d::Clamp(numGuns, { 1, 5 });
		world.AddProjectileLauncherComponent(entityID, numGuns, false);
		unsigned launcherSlot = 0;
		if(numGuns == 1 || numGuns == 3 || numGuns == 5)
			world.AddProjectileLauncher(entityID, launcherSlot++, m_models.bulletDef, { BLASTER_PROJECTILE_OFFSET_X, 0.0f }, BLASTER_CANON_IMPULSE, BLASTER_CANON_INTERVAL, false, false);
		if(numGuns >= 2)
		{
			world.AddProjectileLauncher(entityID, launcherSlot++, m_models.bulletDef, { BLASTER_PROJECTILE_OFFSET_X,  BLASTER_PROJECTILE_INNER_SPREAD_Y }, BLASTER_CANON_IMPULSE, BLASTER_CANON_INTERVAL, false, false);
			world.AddProjectileLauncher(entityID, launcherSlot++, m_models.bulletDef, { BLASTER_PROJECTILE_OFFSET_X, -BLASTER_PROJECTILE_INNER_SPREAD_Y }, BLASTER_CANON_IMPULSE, BLASTER_CANON_INTERVAL, false, false);
		}
		if(numGuns >= 4)
		{
			world.AddProjectileLauncher(entityID, launcherSlot++, m_models.bulletDef, { BLASTER_PROJECTILE_OFFSET_X,  BLASTER_PROJECTILE_OUTER_SPREAD_Y }, BLASTER_CANON_IMPULSE, BLASTER_CANON_INTERVAL, false, false);
			world.AddProjectileLauncher(entityID, launcherSlot++, m_models.bulletDef, { BLASTER_PROJECTILE_OFFSET_X, -BLASTER_PROJECTILE_OUTER_SPREAD_Y }, BLASTER_CANON_IMPULSE, BLASTER_CANON_INTERVAL, false, false);
		}
	}

	//+---------------------------\-------------------------------
	//|		 CreateUFOGray		  |
	//\---------------------------/-------------------------------
	WorldID EntityFactory::CreateUFOGray(World &world, const InstanceDef &def)
	{
		b2Vec2 size{ UFO_HEIGHT * m_models.textures.ufoGray.GetWidthToHeightRatio(), UFO_HEIGHT };
		WorldID id = CreateBasicObject(world, size, DEFAULT_DRAW_LAYER, m_models.ufoGray, SHIP_MATERIAL, SHIP_FILTER, b2_dynamicBody, def);
		world.AddRotatorComponent(id, UFO_ROTATION_SPEED);
		world.AddThrusterComponent(id, 1, 1.0f);
		world.AddThruster(id, 0, m_models.blasterThruster.animationDef, UFO_THRUSTER_ACCELERATION, 0.0f, { BLASTER_THRUSTER_OFFSET_X, 0.0f });

		world.AddHealthComponent(id, UFO_HP);
		world.AddParticleExplosionOnDeathComponent(id, PARTICLE_EXPLOSION_RELATIVE_SIZE,
			UFO_NUM_PARTICLES, UFO_PARTICLE_SPEED_RANGE, DAMAGE_BASED_SPEED_INCREASE_FACTOR,
			UFO_PARTICLE_SIZE_INDEX_RANGE, UFO_GRAY_PARTICLE_COLOR_RANGE,
			UFO_PARTICLE_LIFETIME, PARTICLE_EXPLOSION_FADEIN, BLASTER_PARTICLE_FADEOUT);
		return id;
	}

	//+--------------------------------\--------------------------
	//|	  CreateRandomXLargeAsteroids  |
	//\--------------------------------/--------------------------
	std::vector<WorldID> EntityFactory::CreateRandomXLargeAsteroids(World& world, unsigned count, float directionAngle)
	{
		const float minGap = XLARGE_ASTEROID_HEIGHT * MIN_BOUNDING_RADII_GAP_RELATIVE_TO_HEIGHT;
		const b2Vec2 direction = d2d::GetUnitVec2FromAngle(directionAngle);
		std::vector<WorldID> idList;
		for(unsigned i = 0; i < count; ++i)
		{
			int model = d2d::RandomInt({ 0, NUM_XLARGE_ASTEROID_MODELS - 1 });
			b2Vec2 size;
			size.y = XLARGE_ASTEROID_HEIGHT * XLARGE_ASTEROID_RELATIVE_HEIGHTS[model];
			size.x = size.y * m_models.textures.asteroidsXLarge[model].GetWidthToHeightRatio();
			float boundingRadius = size.Length() * 0.5f;

			float speed = d2d::RandomFloat(ASTEROID_STARTING_SPEED_RANGE_XL);
			InstanceDef def{
				.angle = d2d::RandomFloat({ 0.0f, d2d::TWO_PI }),
				.velocity{speed * direction},
				.angularVelocity = d2d::RandomFloat(ASTEROID_STARTING_ANG_VEL_RANGE_XL) };
			bool positionFound =
				world.GetRandomPositionAwayFromExistingEntities(
					boundingRadius, minGap, MAX_ATTEMPTS_PER_ENTITY, def.position);
			if(positionFound)
				idList.push_back(CreateXLargeAsteroid(world, model, d2d::RandomBool(), def));
			else
				throw CouldNotPlaceEntityException{"CreateRandomIcon"};
		}
		return idList;
	}

	//+--------------------------------\--------------------------
	//|	  CreateRandomLargeAsteroids   |
	//\--------------------------------/--------------------------
	std::vector<WorldID> EntityFactory::CreateRandomLargeAsteroids(World& world, unsigned count, float directionAngle)
	{
		const float minGap = LARGE_ASTEROID_HEIGHT * MIN_BOUNDING_RADII_GAP_RELATIVE_TO_HEIGHT;
		const b2Vec2 direction = d2d::GetUnitVec2FromAngle(directionAngle);
		std::vector<WorldID> idList;
		for(unsigned i = 0; i < count; ++i)
		{
			int model = d2d::RandomInt({ 0, NUM_LARGE_ASTEROID_MODELS - 1 });
			b2Vec2 size;
			size.y = LARGE_ASTEROID_HEIGHT * LARGE_ASTEROID_RELATIVE_HEIGHTS[model];
			size.x = size.y * m_models.textures.asteroidsLarge[model].GetWidthToHeightRatio();
			float boundingRadius = size.Length() * 0.5f;

			float speed = d2d::RandomFloat(ASTEROID_STARTING_SPEED_RANGE_L);
			InstanceDef def{
				.angle = d2d::RandomFloat({ 0.0f, d2d::TWO_PI }),
				.velocity{speed * direction},
				.angularVelocity = d2d::RandomFloat(ASTEROID_STARTING_ANG_VEL_RANGE_L) };
			bool positionFound =
				world.GetRandomPositionAwayFromExistingEntities(
					boundingRadius, minGap, MAX_ATTEMPTS_PER_ENTITY, def.position);
			if(positionFound)
				CreateLargeAsteroid(world, model, d2d::RandomBool(), def);
			else
				throw CouldNotPlaceEntityException{"CreateRandomIcon"};
		}
		return idList;
	}

	//+--------------------------------\--------------------------
	//|	 CreateRandomMediumAsteroids   |
	//\--------------------------------/--------------------------
	std::vector<WorldID> EntityFactory::CreateRandomMediumAsteroids(World& world, unsigned count, float directionAngle)
	{
		const float minGap = MEDIUM_ASTEROID_HEIGHT * MIN_BOUNDING_RADII_GAP_RELATIVE_TO_HEIGHT;
		const b2Vec2 direction = d2d::GetUnitVec2FromAngle(directionAngle);
		std::vector<WorldID> idList;
		for(unsigned i = 0; i < count; ++i)
		{
			int model = d2d::RandomInt({ 0, NUM_MEDIUM_ASTEROID_MODELS - 1 });
			b2Vec2 size;
			size.y = MEDIUM_ASTEROID_HEIGHT * MEDIUM_ASTEROID_RELATIVE_HEIGHTS[model];
			size.x = size.y * m_models.textures.asteroidsMedium[model].GetWidthToHeightRatio();
			float boundingRadius = size.Length() * 0.5f;

			float speed = d2d::RandomFloat(ASTEROID_STARTING_SPEED_RANGE_M);
			InstanceDef def{
				.angle = d2d::RandomFloat({ 0.0f, d2d::TWO_PI }),
				.velocity{speed * direction},
				.angularVelocity = d2d::RandomFloat(ASTEROID_STARTING_ANG_VEL_RANGE_M) };
			bool positionFound =
				world.GetRandomPositionAwayFromExistingEntities(
					boundingRadius, minGap, MAX_ATTEMPTS_PER_ENTITY, def.position);
			if(positionFound)
				CreateMediumAsteroid(world, model, d2d::RandomBool(), def);
			else
				throw CouldNotPlaceEntityException{"CreateRandomIcon"};
		}
		return idList;
	}

	//+--------------------------------\--------------------------
	//|	  CreateRandomSmallAsteroids   |
	//\--------------------------------/--------------------------
	std::vector<WorldID> EntityFactory::CreateRandomSmallAsteroids(World& world, unsigned count, float directionAngle)
	{
		const float minGap = SMALL_ASTEROID_HEIGHT * MIN_BOUNDING_RADII_GAP_RELATIVE_TO_HEIGHT;
		const b2Vec2 direction = d2d::GetUnitVec2FromAngle(directionAngle);
		std::vector<WorldID> idList;
		for(unsigned i = 0; i < count; ++i)
		{
			int model = d2d::RandomInt({ 0, NUM_SMALL_ASTEROID_MODELS - 1 });
			b2Vec2 size;
			size.y = SMALL_ASTEROID_HEIGHT * SMALL_ASTEROID_RELATIVE_HEIGHTS[model];
			size.x = size.y * m_models.textures.asteroidsSmall[model].GetWidthToHeightRatio();
			float boundingRadius = size.Length() * 0.5f;

			float speed = d2d::RandomFloat(ASTEROID_STARTING_SPEED_RANGE_S);
			InstanceDef def{
				.angle = d2d::RandomFloat({ 0.0f, d2d::TWO_PI }),
				.velocity{speed * direction},
				.angularVelocity = d2d::RandomFloat(ASTEROID_STARTING_ANG_VEL_RANGE_S) };
			bool positionFound =
				world.GetRandomPositionAwayFromExistingEntities(
					boundingRadius, minGap, MAX_ATTEMPTS_PER_ENTITY, def.position);
			if(positionFound)
				CreateSmallAsteroid(world, model, d2d::RandomBool(), def);
		}
		return idList;
	}

	//+---------------------------\-------------------------------
	//|	   CreateXLargeAsteroid	  |
	//\---------------------------/-------------------------------
	WorldID EntityFactory::CreateXLargeAsteroid(World& world, unsigned modelIndex, bool isRock, const InstanceDef& def)
	{
		d2Assert(modelIndex < NUM_XLARGE_ASTEROID_MODELS);
		float height{ XLARGE_ASTEROID_HEIGHT * XLARGE_ASTEROID_RELATIVE_HEIGHTS[modelIndex] };
		b2Vec2 size{ height * m_models.textures.asteroidsXLarge[modelIndex].GetWidthToHeightRatio(), height };
		const Model& model = isRock ? m_models.rocksXLarge.at(modelIndex) : m_models.asteroidsXLarge.at(modelIndex);
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
	WorldID EntityFactory::CreateLargeAsteroid(World& world, unsigned modelIndex, bool isRock, const InstanceDef& def)
	{
		d2Assert(modelIndex < NUM_LARGE_ASTEROID_MODELS);
		float height{ LARGE_ASTEROID_HEIGHT * LARGE_ASTEROID_RELATIVE_HEIGHTS[modelIndex] };
		b2Vec2 size{ height * m_models.textures.asteroidsLarge[modelIndex].GetWidthToHeightRatio(), height };
		const Model& model = isRock ? m_models.rocksLarge.at(modelIndex) : m_models.asteroidsLarge.at(modelIndex);
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
	WorldID EntityFactory::CreateMediumAsteroid(World& world, unsigned modelIndex, bool isRock, const InstanceDef& def)
	{
		d2Assert(modelIndex < NUM_MEDIUM_ASTEROID_MODELS);
		float height{ MEDIUM_ASTEROID_HEIGHT * MEDIUM_ASTEROID_RELATIVE_HEIGHTS[modelIndex] };
		b2Vec2 size{ height * m_models.textures.asteroidsMedium[modelIndex].GetWidthToHeightRatio(), height };
		const Model& model = isRock ? m_models.rocksMedium.at(modelIndex) : m_models.asteroidsMedium.at(modelIndex);
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
	WorldID EntityFactory::CreateSmallAsteroid(World& world, unsigned modelIndex, bool isRock, const InstanceDef& def)
	{
		d2Assert(modelIndex < NUM_SMALL_ASTEROID_MODELS);
		float height{ SMALL_ASTEROID_HEIGHT * SMALL_ASTEROID_RELATIVE_HEIGHTS[modelIndex] };
		b2Vec2 size{ height * m_models.textures.asteroidsSmall[modelIndex].GetWidthToHeightRatio(), height };
		const Model& model = isRock ? m_models.rocksSmall.at(modelIndex) : m_models.asteroidsSmall.at(modelIndex);
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
	WorldID EntityFactory::CreateBumper(World& world, const InstanceDef& def)
	{
		b2Vec2 size{ BUMPER_HEIGHT * m_models.textures.bumper.GetWidthToHeightRatio(), BUMPER_HEIGHT };
		WorldID id = CreateBasicObject(world, size, DEFAULT_DRAW_LAYER, m_models.bumper, BUMPER_MATERIAL, BUMPER_FILTER, b2_kinematicBody, def);
		return id;
	}

	//+---------------------------\-------------------------------
	//|	       CreateSoda		  |
	//\---------------------------/-------------------------------
	WorldID EntityFactory::CreateSoda(World& world, const InstanceDef& def)
	{
		b2Vec2 size{ SODA_HEIGHT * m_models.textures.soda.GetWidthToHeightRatio(), SODA_HEIGHT };
		WorldID id = CreateBasicObject(world, size, DEFAULT_DRAW_LAYER, m_models.soda, FUEL_MATERIAL, FUEL_FILTER, b2_dynamicBody, def);
		PowerUpComponent powerUp;
		powerUp.type = PowerUpType::FUEL;
		powerUp.value = 10;
		world.AddPowerUpComponent(id, powerUp);
		return id;
	}

	//+---------------------------\-------------------------------
	//|		   CreateMelon		  |
	//\---------------------------/-------------------------------
	WorldID EntityFactory::CreateMelon(World& world, const InstanceDef& def)
	{
		b2Vec2 size{ MELON_HEIGHT * m_models.textures.melon.GetWidthToHeightRatio(), MELON_HEIGHT };
		WorldID id = CreateBasicObject(world, size, DEFAULT_DRAW_LAYER, m_models.melon, FUEL_MATERIAL, FUEL_FILTER, b2_dynamicBody, def);
		PowerUpComponent powerUp;
		powerUp.type = PowerUpType::FUEL;
		powerUp.value = 20;
		world.AddPowerUpComponent(id, powerUp);
		return id;
	}

	//+---------------------------\-------------------------------
	//|		   CreateApple		  |
	//\---------------------------/-------------------------------
	WorldID EntityFactory::CreateApple(World& world, const InstanceDef& def)
	{
		b2Vec2 size{ APPLE_HEIGHT * m_models.textures.apple.GetWidthToHeightRatio(), APPLE_HEIGHT };
		size = 4.0f * size;
		WorldID id = CreateBasicObject(world, size, DEFAULT_DRAW_LAYER, m_models.apple, FUEL_MATERIAL, FUEL_FILTER, b2_dynamicBody, def);
		PowerUpComponent powerUp;
		powerUp.type = PowerUpType::FUEL;
		powerUp.value = 5;
		world.AddPowerUpComponent(id, powerUp);
		return id;
	}

	//+---------------------------\-------------------------------
	//|		CreateRandomIcons	  |
	//\---------------------------/-------------------------------
	std::vector<WorldID> EntityFactory::CreateRandomIcons(World& world, unsigned count)
	{
		std::vector<WorldID> idList;
		for(unsigned i = 0; i < count; ++i)
		{
			const float minGap = MIN_BOUNDING_RADII_GAP_RELATIVE_TO_HEIGHT_ICONS * ICON_HEIGHT;
			int model{ d2d::RandomInt({0, NUM_ICON_MODELS - 1}) };
			b2Vec2 size;
			size.y = ICON_HEIGHT;
			size.x = size.y * m_models.textures.icons[model].GetWidthToHeightRatio();
			float boundingRadius = size.Length() * 0.5f;

			InstanceDef def{ .angle = 0.0f };
			bool positionFound =
				world.GetRandomPositionAwayFromExistingEntities(
					boundingRadius, minGap, MAX_ATTEMPTS_PER_ENTITY, def.position);
			if(positionFound)
				idList.push_back(CreateIcon(world, model, def));
			else
				throw CouldNotPlaceEntityException{"CreateRandomIcon"};
		}
		return idList;
	}

	//+---------------------------\-------------------------------
	//|		   CreateIcon		  |
	//\---------------------------/-------------------------------
	WorldID EntityFactory::CreateIcon(World& world, unsigned modelIndex, const InstanceDef& def)
	{
		d2Assert(modelIndex < m_models.textures.icons.size());
		b2Vec2 size{ ICON_HEIGHT * m_models.textures.icons.at(0).GetWidthToHeightRatio(), ICON_HEIGHT };
		Model model{ "icon", &m_models.textures.icons.at(modelIndex)};
		WorldID id = CreateBasicObject(world, size, DEFAULT_DRAW_LAYER, model, ICON_MATERIAL, ICON_FILTER, b2_dynamicBody, def);
		PowerUpComponent powerUp;
		powerUp.type = PowerUpType::ICON;
		powerUp.value = 1;
		world.AddPowerUpComponent(id, powerUp);
		return id;
	}

	//+---------------------------\-------------------------------
	//|	      CreateExit		  |
	//\---------------------------/-------------------------------
	WorldID EntityFactory::CreateExit(World& world, const InstanceDef& def)
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
	WorldID EntityFactory::CreateExitSensor(World& world, const InstanceDef& def)
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

}
