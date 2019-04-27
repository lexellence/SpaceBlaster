/**************************************************************************************\
** File: GameDef.h
** Project: 
** Author: David Leksen
** Date: 
**
** Header file for the GameDef struct
**
\**************************************************************************************/
#pragma once
#include "Starfield.h"
namespace Space
{
	struct EntityDef
	{

	};
	struct EntityInstance
	{

	};
	struct Objective
	{

	};
	struct Quest
	{
		std::vector<Objective> objectiveList;
	};
	struct QuestSequence
	{
		std::vector<Quest> questList;
	};
	struct LevelDef
	{
		std::vector<EntityInstance> entityList;
		std::vector<QuestSequence> missionSequenceList;
	};
	struct GameDef
	{
		void LoadFrom(const std::string& filePath);
		void Validate() const;

		std::string firstLevel;
		d2d::Range<float> cameraDimensionRange;
		float cameraZoomSpeed;
		float cameraInitialZoomOutPercent;

		StarfieldDef starfield;
		std::map<std::string, EntityDef> entityDefMap;
		std::map<std::string, LevelDef> levelDefMap;
	};
}
