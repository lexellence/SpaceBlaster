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
		WorldID CreateBasicObject(World &world, const b2Vec2 &size, int drawLayer,
			const Model &model, const d2d::Material &material, const d2d::Filter &filter,
			b2BodyType physicsType, const InstanceDef &def);
		WorldID CreateScout(World &world, const InstanceDef &def);
		WorldID CreateBlaster(World &world, const InstanceDef &def);
		void AddBlasterGuns(World& world, WorldID entityID, unsigned count);
		std::vector<WorldID> CreateRandomXLargeAsteroids(World& world, unsigned count, float directionAngle);
		std::vector<WorldID> CreateRandomLargeAsteroids(World& world, unsigned count, float directionAngle);
		std::vector<WorldID> CreateRandomMediumAsteroids(World& world, unsigned count, float directionAngle);
		std::vector<WorldID> CreateRandomSmallAsteroids(World& world, unsigned count, float directionAngle);

		WorldID CreateXLargeAsteroid(World &world, unsigned modelIndex, bool isRock, const InstanceDef &def);
		WorldID CreateLargeAsteroid(World &world, unsigned modelIndex, bool isRock, const InstanceDef &def);
		WorldID CreateMediumAsteroid(World &world, unsigned modelIndex, bool isRock, const InstanceDef &def);
		WorldID CreateSmallAsteroid(World &world, unsigned modelIndex, bool isRock, const InstanceDef &def);
		WorldID CreateBumper(World &world, const InstanceDef &def);
		WorldID CreateSoda(World &world, const InstanceDef &def);
		WorldID CreateMelon(World &world, const InstanceDef &def);
		WorldID CreateApple(World &world, const InstanceDef &def);
		std::vector<WorldID> CreateRandomIcons(World& world, unsigned numIcons);
		WorldID CreateIcon(World &world, unsigned modelIndex, const InstanceDef &def);
		WorldID CreateExit(World &world, const InstanceDef &def);
		WorldID CreateExitSensor(World &world, const InstanceDef &def);
	private:
		GameModels m_models;
	};
}
