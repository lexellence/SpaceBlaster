/**************************************************************************************\
** File: ClassTemplate.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the ClassTemplate class
**
\**************************************************************************************/
#include "pch.h"
#include "World.h"
#include "WorldUtility.h"
namespace Space
{
	//+----------------------\------------------------------------
	//|	   AddAIComponent	 |
	//\----------------------/------------------------------------
	void World::AddAIComponent(EntityID entityID, AIType type)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_componentBits[entityID].set(COMPONENT_AI);
		m_AIComponents[entityID].type = type;
	}

	//+----------------------\------------------------------------
	//|	 UpdateAIComponents	 |
	//\----------------------/------------------------------------
	void World::UpdateAIComponents(float dt)
	{
		for(EntityID id = 0; id < WORLD_MAX_ENTITIES; ++id)
			if(HasComponentSet(id, COMPONENT_AI) && IsActive(id))
			{
				if(m_AIComponents[id].type == AIType::AI_ROAM)
				{
					// Turn to avoid collision
					for(EntityID id : GetEntitiesInArea(m_smoothedTransforms[id].p, 20.0f))
					{
						
					}

					// Thrust until ideal speed
				}
			}
	}
}