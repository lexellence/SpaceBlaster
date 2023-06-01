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
#include "World.h"
namespace Space
{
	struct EntityDef
	{

	};
	struct EntityInstance
	{

	};
	enum class ObjectiveType
	{
		DESTROY, COLLECT
	};
	struct Objective
	{
		ObjectiveType type;
		std::string text;
		std::vector<WorldID> entityIDs;
		int num;
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

		// Both width and height will be restricted
		//	to this range regardless of aspect ratio.
		d2d::Range<float> cameraDimensionRange;
		float cameraZoomSpeed;
		float cameraInitialZoomOutPercent;

		StarfieldDef starfield;
		std::map<std::string, EntityDef> entityDefMap;
		std::map<std::string, LevelDef> levelDefMap;
	};
}
