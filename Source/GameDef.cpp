/**************************************************************************************\
** File: GameDef.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the GameDef struct
**
\**************************************************************************************/
#include "pch.h"
#include "GameDef.h"
#include "Starfield.h"
#include "Exceptions.h"

namespace Space
{
	void GameDef::LoadFrom(const std::string& gameFilePath)
	{
		d2d::HjsonValue data{ d2d::FileToHJSON(gameFilePath) };
		if(!d2d::IsNonNull(data))
			throw LoadSettingsFileException{ gameFilePath + ": Invalid file" };

		// Get root level values
		d2d::HjsonValue starfieldData;
		//d2d::HjsonValue texturesData;
		//d2d::HjsonValue constantsData;
		//d2d::HjsonValue entityDefGroupsData;
		//d2d::HjsonValue entityDefsData;
		//d2d::HjsonValue levelsData;
		try {
			cameraDimensionRange.Set(d2d::GetVectorFloat(data, "cameraDimensionRange", 0),
				d2d::GetVectorFloat(data, "cameraDimensionRange", 1));
			cameraZoomSpeed = d2d::GetFloat(data, "cameraZoomSpeed");
			cameraInitialZoomOutPercent = d2d::GetClampedPercent(d2d::GetFloat(data, "cameraInitialZoomOutPercent"));

			starfieldData = d2d::GetMemberValue(data, "starfield");
			//texturesData = d2d::GetMemberValue(data, "textures");
			//constantsData = d2d::GetMemberValue(data, "constants");
			//entityDefGroupsData = d2d::GetMemberValue(data, "entityDefGroups");
			//entityDefsData = d2d::GetMemberValue(data, "entityDefs");
			//levelsData = d2d::GetMemberValue(data, "levels");
		}
		catch(const d2d::HjsonFailedQueryException& e) {
			throw LoadSettingsFileException{ gameFilePath + ": Invalid value: " + e.what() };
		}

		// Get starfield settings
		try {
			starfield.maxCameraDimension = cameraDimensionRange.GetMax();
			starfield.edgePaddingFactor = d2d::GetFloat(starfieldData, "edgePaddingFactor");
			starfield.density = d2d::GetFloat(starfieldData, "density");
			starfield.speedFactorRange.Set(d2d::GetVectorFloat(starfieldData, "speedFactorRange", 0),
				d2d::GetVectorFloat(starfieldData, "speedFactorRange", 1));
			starfield.pointSizeIndexRange.Set(d2d::GetVectorInt(starfieldData, "pointSizeIndexRange", 0),
				d2d::GetVectorInt(starfieldData, "pointSizeIndexRange", 1));
			starfield.maxPointSizeIndexVariation = d2d::GetInt(starfieldData, "maxPointSizeIndexVariation");
			starfield.colorRange.Set(d2d::GetColorFloat(starfieldData, "colorMin"),
				d2d::GetColorFloat(starfieldData, "colorMax"));
			starfield.maxAlphaVariation = d2d::GetFloat(starfieldData, "maxAlphaVariation");
		}
		catch(const d2d::HjsonFailedQueryException& e) {
			throw LoadSettingsFileException{ gameFilePath + ": Invalid value: starfield." + e.what() };
		}

		// textures
		// constants
		// entities
		// levels

		try {
			Validate();
		}
		catch(const SettingOutOfRangeException& e) {
			throw LoadSettingsFileException{ gameFilePath + ": Game setting out of range: " + e.what() };
		}
	}
	void GameDef::Validate() const
	{
		if(cameraDimensionRange.GetMin() <= 0.0f) throw SettingOutOfRangeException{ "cameraDimensionRange[0]" };
		if(cameraZoomSpeed < 0.0f) throw SettingOutOfRangeException{ "cameraZoomSpeed" };
		if(!d2d::VALID_PERCENT_RANGE.Contains(cameraInitialZoomOutPercent)) throw SettingOutOfRangeException{ "cameraInitialZoomOutPercent" };
		if(starfield.edgePaddingFactor < 0.0f) throw SettingOutOfRangeException{ "starfield.edgePaddingFactor" };
		if(starfield.density < 0.0f) throw SettingOutOfRangeException{ "starfield.density" };
		if(starfield.speedFactorRange.GetMin() < 0.0f) throw SettingOutOfRangeException{ "starfield.speedFactorRange[0]" };
		if(!d2d::Window::VALID_POINT_SIZES.Contains(starfield.pointSizeIndexRange)) throw SettingOutOfRangeException{ "starfield.pointSizeIndexRange" };
		if(starfield.maxAlphaVariation < 0.0f) throw SettingOutOfRangeException{ "starfield.maxAlphaVariation" };
		if(starfield.maxPointSizeIndexVariation < 0) throw SettingOutOfRangeException{ "starfield.maxPointSizeIndexVariation" };
		//if(entityDefMap.empty()) throw SettingOutOfRangeException{ "starfield.entityDefMap" } ;
		//if(levelDefMap.empty()) throw SettingOutOfRangeException{ "starfield.levelDefMap" };
	}
}
