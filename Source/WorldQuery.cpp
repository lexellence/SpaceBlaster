/**************************************************************************************\
** File: WorldQuery.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the query functions of the World class
**
\**************************************************************************************/
#include "pch.h"
#include "World.h"

namespace Space
{
	//+----------------------\------------------------------------
	//|		GetWorldRect	 |
	//\----------------------/------------------------------------
	const d2d::Rect& World::GetWorldRect() const
	{
		return m_worldRect;
	}
	//+----------------------\------------------------------------
	//|	   GetWorldCenter	 |
	//\----------------------/------------------------------------
	const b2Vec2& World::GetWorldCenter() const
	{
		return m_worldCenter;
	}
	//+----------------------\------------------------------------
	//|	   GetEntityCount	 |
	//\----------------------/------------------------------------
	EntityID World::GetEntityCount() const
	{
		EntityID entityCount{ 0 };
		for(EntityID id = 0; id < WORLD_MAX_ENTITIES; ++id)
			if(EntityExists(id))
				++entityCount;
		return entityCount;
	}
	//+----------------------\------------------------------------
	//|	   EntityExists 	 |
	//\----------------------/------------------------------------
	bool World::EntityExists(EntityID entityID) const
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		return m_componentBits[entityID].any() || m_flagBits[entityID].any();
	}
	//+----------------------\------------------------------------
	//|	    HasComponent	 |
	//\----------------------/------------------------------------
	bool World::HasComponent(EntityID entityID, ComponentBit component) const
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		return m_componentBits[entityID].test(component);
	}
	//+----------------------\------------------------------------
	//|	    HasComponents	 |
	//\----------------------/------------------------------------
	bool World::HasComponentSet(EntityID entityID, ComponentBitset componentBits) const
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		return (m_componentBits[entityID] & componentBits) == componentBits;
	}
	//+----------------------\------------------------------------
	//|		  HasFlag		 |
	//\----------------------/------------------------------------
	bool World::HasFlag(EntityID entityID, FlagBit flagBit) const
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		return m_flagBits[entityID].test(flagBit);
	}
	//+----------------------\------------------------------------
	//|		 HasFlags		 |
	//\----------------------/------------------------------------
	bool World::HasFlagSet(EntityID entityID, FlagBitset flagBits) const
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		return (m_flagBits[entityID] & flagBits) == flagBits;
	}
	//+----------------------\------------------------------------
	//|		 IsActive		 |
	//\----------------------/------------------------------------
	bool World::IsActive(EntityID entityID) const
	{
		return HasFlag(entityID, FLAG_ACTIVE);
	}
	//+----------------------\------------------------------------
	//|		  HasSize		 |
	//\----------------------/------------------------------------
	bool World::HasSize(EntityID entityID) const
	{
		return EntityExists(entityID) &&
			(m_sizeComponents[entityID].x > 0.0f ||
				m_sizeComponents[entityID].y > 0.0f);
	}
	//+----------------------\------------------------------------
	//|		 HasSize2D		 |
	//\----------------------/------------------------------------
	bool World::HasSize2D(EntityID entityID) const
	{
		return EntityExists(entityID) &&
			(m_sizeComponents[entityID].x > 0.0f &&
				m_sizeComponents[entityID].y > 0.0f);
	}
	//+----------------------\------------------------------------
	//|		 HasPhysics		 |
	//\----------------------/------------------------------------
	bool World::HasPhysics(EntityID entityID) const
	{
		return HasComponent(entityID, COMPONENT_PHYSICS);
	}
	//+------------------------------\----------------------------
	//|   GetClosestPhysicalEntity	 |
	//\------------------------------/
	//	Returns false if no physical entities exist, otherwise
	//	returns true and sets entityIDOut and relativePositionOut to
	//	the closest entity (relativePosition may point to clone body)
	//+-----------------------------------------------------------
	bool World::GetClosestPhysicalEntity(const b2Vec2& position, float boundingRadius,
		EntityID& entityIDOut, float& boundingRadiiGapOut) const
	{
		b2Body* b2BodyPtr = m_b2WorldPtr->GetBodyList();
		if(!b2BodyPtr)
			return false;

		EntityID closestEntityID;
		float closestGap{ FLT_MAX };
		while(b2BodyPtr)
		{
			Body* bodyPtr = GetUserBodyPtr(b2BodyPtr);
			if(bodyPtr)
			{
				b2Vec2 relativePosition{ b2BodyPtr->GetPosition() - position };
				float boundingCircleRadius;
				if(HasSize(bodyPtr->entityID))
					boundingCircleRadius = m_boundingRadiusComponents[bodyPtr->entityID];
				else
					boundingCircleRadius = 0.0f;
				float gap{ relativePosition.Length() - boundingRadius - boundingCircleRadius };
				if(gap < closestGap)
				{
					closestEntityID = bodyPtr->entityID;
					closestGap = gap;
				}
				b2BodyPtr = b2BodyPtr->GetNext();
			}
		}

		entityIDOut = closestEntityID;
		boundingRadiiGapOut = closestGap;
		return true;
	}
	int World::GetDrawLayer(EntityID entityID) const
	{
		if(EntityExists(entityID))
			return m_drawAnimationComponents[entityID].layer;
		else
			return 0;
	}
	float World::GetFuelLevel(EntityID entityID) const
	{
		return m_fuelComponents[entityID].level;
	}
	float World::GetMaxFuelLevel(EntityID entityID) const
	{
		return m_fuelComponents[entityID].max;
	}
	float World::GetTotalThrusterAcceleration(EntityID id) const
	{
		float totalAcceleration{ 0.0f };
		if(HasComponent(id, COMPONENT_THRUSTER) && IsActive(id))
		{
			for(unsigned i = 0; i < m_thrusterComponents[id].numSlots; ++i)
				if(m_thrusterComponents[id].thrusters[i].enabled)
					totalAcceleration += m_thrusterComponents[id].thrusters[i].acceleration;
			// Boost
			if(HasComponent(id, COMPONENT_BOOSTER))
				if(m_boosterComponents[id].secondsLeft > 0.0f)
					totalAcceleration *= m_boosterComponents[id].factor;
		}
		return totalAcceleration;
	}
	float World::GetTotalThrusterFuelRequired(EntityID id, float dt) const
	{
		float totalFuelRequired{ 0.0f };
		if(HasComponent(id, COMPONENT_THRUSTER) && IsActive(id))
		{
			for(unsigned i = 0; i < m_thrusterComponents[id].numSlots; ++i)
				if(m_thrusterComponents[id].thrusters[i].enabled)
					totalFuelRequired += dt * m_thrusterComponents[id].thrusters[i].fuelPerSecond * m_thrusterComponents[id].factor;
			// Boost
			if(HasComponent(id, COMPONENT_BOOSTER))
				if(m_boosterComponents[id].secondsLeft > 0.0f)
					totalFuelRequired *= m_boosterComponents[id].factor * WORLD_BOOST_FUEL_USE_PENALTY_FACTOR;

		}
		//std::cout << "totalFuelRequired = " << totalFuelRequired << std::endl;
		return totalFuelRequired;
	}

	//+-------------------------\---------------------------------------------
	//|	 GetSmoothedTransform	|
	//\-------------------------/
	//	Failure to ensure entity has a physics component will result in undefined behavior
	//+-----------------------------------------------------------------------
	const b2Transform& World::GetSmoothedTransform(EntityID entityID) const
	{
		d2Assert(HasPhysics(entityID));
		return m_smoothedTransforms[entityID];
	}
	//+-------------------------\---------------------------------------------
	//|	  GetLinearVelocity		|
	//\-------------------------/
	//	Failure to ensure entity has a physics component will result in undefined behavior
	//+-----------------------------------------------------------------------
	const b2Vec2& World::GetLinearVelocity(EntityID entityID) const
	{
		d2Assert(HasPhysics(entityID));
		return m_physicsComponents[entityID].mainBody.b2BodyPtr->GetLinearVelocity();
	}
	//+-------------------------\---------------------------------------------
	//|	  GetAngularVelocity	|
	//\-------------------------/
	//	Failure to ensure entity has a physics component will result in undefined behavior
	//+-----------------------------------------------------------------------
	float World::GetAngularVelocity(EntityID entityID) const
	{
		d2Assert(HasPhysics(entityID));
		return m_physicsComponents[entityID].mainBody.b2BodyPtr->GetAngularVelocity();
	}
	//+-------------------------\---------------------------------------------
	//|	 GetLocalCenterOfMass	|
	//\-------------------------/
	//	Failure to ensure entity has a physics component will result in undefined behavior
	//+-----------------------------------------------------------------------
	const b2Vec2& World::GetLocalCenterOfMass(EntityID entityID) const
	{
		//std::cout << "World::GetLocalCenterOfMass entityID=" << entityID << std::endl;
		d2Assert(HasPhysics(entityID));
		return m_physicsComponents[entityID].mainBody.b2BodyPtr->GetLocalCenter();
	}
	//+---------------------\-------------------------------------------------
	//|	   GetWorldCenter	|
	//\---------------------/
	//	Failure to ensure entity has a physics component will result in undefined behavior
	//+-----------------------------------------------------------------------
	const b2Vec2& World::GetWorldCenter(EntityID entityID) const
	{
		d2Assert(HasPhysics(entityID));
		return m_physicsComponents[entityID].mainBody.b2BodyPtr->GetWorldCenter();
	}

	//**********************  Private Functions  *****************************

	//+-----------------\-----------------------------------------------------
	//|   GetQuadrant	| (private)
	//\-----------------/-----------------------------------------------------
	Quadrant World::GetQuadrant(const b2Vec2& position) const
	{
		bool onLeftHalf{ position.x < m_worldCenter.x };
		bool onBottomHalf{ position.y < m_worldCenter.y };
		if(onLeftHalf)
			return onBottomHalf ? Quadrant::BOTTOM_LEFT : Quadrant::TOP_LEFT;
		else
			return onBottomHalf ? Quadrant::BOTTOM_RIGHT : Quadrant::TOP_RIGHT;
	}
	//+-------------------------\---------------------------------------------
	//|   GetCloneSectionList	| (private)
	//\-------------------------/---------------------------------------------
	CloneSectionList World::GetCloneSectionList(const b2Vec2& position) const
	{
		Quadrant entityQuadrant{ GetQuadrant(position) };
		switch(entityQuadrant)
		{
		case Quadrant::TOP_LEFT:  return { CloneSection::RIGHT, CloneSection::BOTTOM_RIGHT, CloneSection::BOTTOM };
		case Quadrant::TOP_RIGHT: return { CloneSection::BOTTOM, CloneSection::BOTTOM_LEFT, CloneSection::LEFT };
		case Quadrant::BOTTOM_RIGHT: return { CloneSection::LEFT, CloneSection::TOP_LEFT, CloneSection::TOP };
		case Quadrant::BOTTOM_LEFT: default: return { CloneSection::TOP, CloneSection::TOP_RIGHT, CloneSection::RIGHT };
		}
	}
	//+-------------------------\---------------------------------------------
	//|   GetCloneSectionList	| (private)
	//\-------------------------/---------------------------------------------
	CloneSectionList World::GetCloneSectionList(const b2Body& b2Body) const
	{
		return GetCloneSectionList(b2Body.GetWorldCenter());
	}
	//+---------------------\-------------------------------------------------
	//|	  GetCloneOffset	| (private)
	//\---------------------/-------------------------------------------------
	b2Vec2 World::GetCloneOffset(CloneSection cloneSection) const
	{
		switch(cloneSection)
		{
		case CloneSection::TOP_LEFT:		return { -m_worldDimensions.x,  m_worldDimensions.y };
		case CloneSection::TOP:				return { 0.0f,  m_worldDimensions.y };
		case CloneSection::TOP_RIGHT:		return { m_worldDimensions.x,  m_worldDimensions.y };
		case CloneSection::RIGHT:			return { m_worldDimensions.x,				   0.0f };
		case CloneSection::BOTTOM_RIGHT:	return { m_worldDimensions.x, -m_worldDimensions.y };
		case CloneSection::BOTTOM:			return { 0.0f, -m_worldDimensions.y };
		case CloneSection::BOTTOM_LEFT:		return { -m_worldDimensions.x, -m_worldDimensions.y };
		case CloneSection::LEFT: default:	return { -m_worldDimensions.x,				    0.0f };
		}
	}
}
