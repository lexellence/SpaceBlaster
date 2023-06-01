/**************************************************************************************\
** File: WorldDef.h
** Project:
** Author: David Leksen
** Date:
**
** Header file for the WorldDef struct
**
\**************************************************************************************/
#pragma once
namespace Space
{
	struct HealthMeterDef
	{
		float gap;
		float widthPerPoint;
		float height;
		float damagedThreshold;
		float badlyDamagedThreshold;
		d2d::Color backgroundColor;
		d2d::Color hardlyDamagedColor;
		d2d::Color damagedColor;
		d2d::Color badlyDamagedColor;
	};
	struct WorldDef
	{
		void LoadFrom(const std::string& worldFilePath);
		void Validate() const;

		std::string shapeFilePath;
		float drawFixturesLineWidth;
		bool debugDrawFixtures;
		d2d::Range<int> drawLayerRange;
		float stepsPerSecond;
		float maxUpdateTime;
		int velocityIterationsPerStep;
		int positionIterationsPerStep;
		float damageToImpulseRatio;
		float minTotalCollisionDamage;
		bool damageLogging;
		bool addImpulsesForDamages;
		HealthMeterDef healthMeter;
	};
}
