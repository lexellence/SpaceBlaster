/**************************************************************************************\
** File: EntityFactory.h
** Project:
** Author: David Leksen
** Date:
**
** Header file for the EntityFactory class
**
\**************************************************************************************/
#pragma once
#include "World.h"
#include "GameModels.h"
namespace Space
{
	struct GameModels;
	class EntityFactory
	{
	public:
		EntityID CreateBasicObject(World &world, const b2Vec2 &size, int drawLayer,
			const Model &model, const d2d::Material &material, const d2d::Filter &filter,
			b2BodyType physicsType, const InstanceDef &def);
		EntityID CreateScout(World &world, const InstanceDef &def);
		EntityID CreateBlaster(World &world, const InstanceDef &def);
		void AddBlasterGuns(World& world, EntityID entityID, unsigned count);

		EntityID CreateUFOGray(World &world, const InstanceDef &def);

		std::vector<EntityID> CreateRandomXLargeAsteroids(World& world, unsigned count, float directionAngle);
		std::vector<EntityID> CreateRandomLargeAsteroids(World& world, unsigned count, float directionAngle);
		std::vector<EntityID> CreateRandomMediumAsteroids(World& world, unsigned count, float directionAngle);
		std::vector<EntityID> CreateRandomSmallAsteroids(World& world, unsigned count, float directionAngle);

		EntityID CreateXLargeAsteroid(World &world, unsigned modelIndex, bool isRock, const InstanceDef &def);
		EntityID CreateLargeAsteroid(World &world, unsigned modelIndex, bool isRock, const InstanceDef &def);
		EntityID CreateMediumAsteroid(World &world, unsigned modelIndex, bool isRock, const InstanceDef &def);
		EntityID CreateSmallAsteroid(World &world, unsigned modelIndex, bool isRock, const InstanceDef &def);
		EntityID CreateBumper(World &world, const InstanceDef &def);
		EntityID CreateSoda(World &world, const InstanceDef &def);
		EntityID CreateMelon(World &world, const InstanceDef &def);
		EntityID CreateApple(World &world, const InstanceDef &def);
		std::vector<EntityID> CreateRandomIcons(World& world, unsigned numIcons);
		EntityID CreateIcon(World &world, unsigned modelIndex, const InstanceDef &def);
		EntityID CreateExit(World &world, const InstanceDef &def);
		EntityID CreateExitSensor(World &world, const InstanceDef &def);
	private:
		GameModels m_models;
	};
}
