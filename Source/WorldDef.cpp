/**************************************************************************************\
** File: WorldDef.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the WorldDef struct
**
\**************************************************************************************/
#include "pch.h"
#include "WorldDef.h"
#include "Exceptions.h"
namespace Space
{
	//+------------------\----------------------------------------
	//|	  File Loading   |
	//\------------------/----------------------------------------
	void WorldDef::LoadFrom(const std::string& worldFilePath)
	{
		d2d::HjsonValue data{ d2d::FileToHJSON(worldFilePath) };
		if(!d2d::IsNonNull(data))
			throw LoadSettingsFileException{ worldFilePath + ": Invalid file" };

		// Get root level values
		d2d::HjsonValue healthMeterData;
		try {
			shapeFilePath = d2d::GetString(data, "shapeFilePath");
			drawFixturesLineWidth = d2d::GetFloat(data, "drawFixturesLineWidth");
			debugDrawFixtures = d2d::GetBool(data, "debugDrawFixtures");
			drawLayerRange.Set(d2d::GetVectorInt(data, "drawLayerRange", 0),
				d2d::GetVectorInt(data, "drawLayerRange", 1));
			stepsPerSecond = d2d::GetFloat(data, "stepsPerSecond");
			maxUpdateTime = d2d::GetFloat(data, "maxUpdateTime");
			velocityIterationsPerStep = d2d::GetInt(data, "velocityIterationsPerStep");
			positionIterationsPerStep = d2d::GetInt(data, "positionIterationsPerStep");
			damageToImpulseRatio = d2d::GetFloat(data, "damageToImpulseRatio");
			minTotalCollisionDamage = d2d::GetFloat(data, "minTotalCollisionDamage");
			damageLogging = d2d::GetBool(data, "damageLogging");
			addImpulsesForDamages = d2d::GetBool(data, "addImpulsesForDamages");
			healthMeterData = d2d::GetMemberValue(data, "healthMeter");
		}
		catch(const d2d::HjsonFailedQueryException& e) {
			throw LoadSettingsFileException{ worldFilePath + ": Invalid value: " + e.what() };
		}

		// Get healthMeter settings
		try {
			healthMeter.gap = d2d::GetFloat(healthMeterData, "gap");
			healthMeter.widthPerPoint = d2d::GetFloat(healthMeterData, "widthPerPoint");
			healthMeter.height = d2d::GetFloat(healthMeterData, "height");
			healthMeter.damagedThreshold = d2d::GetFloat(healthMeterData, "damagedThreshold");
			healthMeter.badlyDamagedThreshold = d2d::GetFloat(healthMeterData, "badlyDamagedThreshold");

			healthMeter.backgroundColor = d2d::GetColorFloat(healthMeterData, "backgroundColor");
			healthMeter.hardlyDamagedColor = d2d::GetColorFloat(healthMeterData, "hardlyDamagedColor");
			healthMeter.damagedColor = d2d::GetColorFloat(healthMeterData, "damagedColor");
			healthMeter.badlyDamagedColor = d2d::GetColorFloat(healthMeterData, "badlyDamagedColor");
		}
		catch(const d2d::HjsonFailedQueryException& e) {
			throw LoadSettingsFileException{ worldFilePath + ": Invalid value: healthMeter." + e.what() };
		}

		try {
			Validate();
		}
		catch(const SettingOutOfRangeException& e) {
			throw LoadSettingsFileException{ worldFilePath + ": World setting out of range: " + e.what() };
		}
	}
	void WorldDef::Validate() const
	{
		if(shapeFilePath.empty()) throw SettingOutOfRangeException{ "shapeFilePath" };
		if(drawFixturesLineWidth <= 0.0f) throw SettingOutOfRangeException{ "drawFixturesLineWidth" };
		if(stepsPerSecond <= 0.0f) throw SettingOutOfRangeException{ "stepsPerSecond" };
		if(maxUpdateTime <= 0.0f) throw SettingOutOfRangeException{ "maxUpdateTime" };
		if(velocityIterationsPerStep <= 0) throw SettingOutOfRangeException{ "velocityIterationsPerStep" };
		if(positionIterationsPerStep <= 0) throw SettingOutOfRangeException{ "positionIterationsPerStep" };

		// healthMeter
		if(healthMeter.gap < 0.0f) throw SettingOutOfRangeException{ "healthMeter.gap" };
		if(healthMeter.widthPerPoint <= 0.0f) throw SettingOutOfRangeException{ "healthMeter.widthPerPoint" };
		if(healthMeter.height <= 0.0f) throw SettingOutOfRangeException{ "healthMeter.height" };
		if(!d2d::VALID_PERCENT_RANGE.Contains(healthMeter.damagedThreshold)) throw SettingOutOfRangeException{ "healthMeter.damagedThreshold" };
		if(!d2d::VALID_PERCENT_RANGE.Contains(healthMeter.badlyDamagedThreshold)) throw SettingOutOfRangeException{ "healthMeter.badlyDamagedThreshold" };
	}
}