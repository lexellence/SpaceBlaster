/**************************************************************************************\
** File: World.cpp
** Project:
** Author: David Leksen
** Date:
**
** Main source code file for the World class
**
\**************************************************************************************/
#include "pch.h"
#include "World.h"
#include "ParticleSystem.h"
#include "Exceptions.h"
#include "WorldDef.h"

namespace Space
{
	World::~World()
	{
		d2LogDebug << "World used " << m_highestActiveEntityCount << " / " << WORLD_MAX_ENTITIES << " entities. ";
		if(m_b2WorldPtr)
		{
			delete m_b2WorldPtr;
			m_b2WorldPtr = nullptr;
		}
	}
	void World::Init(const d2d::Rect& rect)
	{
		m_settings.LoadFrom("Data/world.hjson");

		// Destroy any existing Box2D physics world
		m_timestepAccumulator = 0.0f;
		if(m_b2WorldPtr)
		{
			delete m_b2WorldPtr;
			m_b2WorldPtr = nullptr;
		}
		while(!m_destroyBuffer.empty())
			m_destroyBuffer.pop();

		// Clear components and flags
		for(WorldID i = 0; i < WORLD_MAX_ENTITIES; ++i)
			m_componentBits[i] = COMPONENT_NONE;
		for(WorldID i = 0; i < WORLD_MAX_ENTITIES; ++i)
			m_flagBits[i] = FLAG_NONE;

		// Create new Box2D physics world
		m_b2WorldPtr = new b2World{ b2Vec2_zero };
		m_b2WorldPtr->SetAutoClearForces(false);
		m_b2WorldPtr->SetContactListener(this);
		m_b2WorldPtr->SetContactFilter(this);

		// World settings/attributes
		m_worldRect = rect;
		m_worldDimensions = rect.GetDimensions();
		m_worldCenter = rect.GetCenter();

		m_particleSystem.Init();

		// Load shape file
		m_shapeFactory.LoadFrom(m_settings.shapeFilePath);
	}
	void World::SetDestructionListener(DestroyListener* listenerPtr)
	{
		m_destructionListenerPtr = listenerPtr;
	}
	void World::SetWrapListener(WrapListener* listenerPtr)
	{
		m_wrappedEntityListenerPtr = listenerPtr;
	}
	void World::SetProjectileLauncherCallback(ProjectileLauncherCallback* callbackPtr)
	{
		m_projectileLauncherCallbackPtr = callbackPtr;
	}
	//+------------------------\----------------------------------
	//|	  Creating Entities    |
	//\------------------------/----------------------------------
	WorldID World::NewEntityID(const b2Vec2& size, int drawLayer, bool activate)
	{
		for(WorldID entityID = 0; entityID < WORLD_MAX_ENTITIES; ++entityID)
			if(m_componentBits[entityID] == COMPONENT_NONE && m_flagBits[entityID] == FLAG_NONE)
			{
				m_sizeComponents[entityID] = size;
				m_boundingRadiusComponents[entityID] = size.Length() * 0.5f;

				if(activate)
					Activate(entityID);

				m_drawAnimationComponents[entityID].layer = drawLayer;
				return entityID;
			}
		throw GameException{ "World ran out of entities" };
	}
	void World::Destroy(WorldID id)
	{
		m_destroyBuffer.push(id);
	}
	void World::SetFlags(WorldID entityID, FlagBits flagBits, bool enable)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		if(enable)
			m_flagBits[entityID] |= flagBits;
		else
			m_flagBits[entityID] &= ~flagBits;
	}
	void World::RemoveComponents(WorldID entityID, BitMask componentBits)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_componentBits[entityID] &= ~componentBits;
	}
	void World::RemoveAllComponentsExcept(WorldID entityID, BitMask componentBits)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_componentBits[entityID] = 0;
		m_componentBits[entityID] |= componentBits;
	}
	//+------------------------\----------------------------------
	//|	  Physics Components   |
	//\------------------------/----------------------------------
	bool World::GetRandomPositionAwayFromExistingEntities(float newBoundingRadius,
		float minGap, unsigned maxAttempts, b2Vec2& positionOut) const
	{
		b2Vec2 position;
		bool acceptablePositionFound{ false };
		unsigned attempts{ 0 };
		do
		{
			position = d2d::RandomVec2InRect(m_worldRect);
			WorldID id;
			float boundingRadiiGap;
			if(!GetClosestPhysicalEntity(position, newBoundingRadius, id, boundingRadiiGap))
				acceptablePositionFound = true;
			else if(boundingRadiiGap >= minGap)
				acceptablePositionFound = true;
		} while(!acceptablePositionFound && ++attempts < maxAttempts);

		if(acceptablePositionFound)
			positionOut = position;

		return acceptablePositionFound;
	}
	void World::AddPhysicsComponent(WorldID entityID, b2BodyType type,
		const InstanceDef& def, bool fixedRotation, bool continuousCollisionDetection)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_componentBits[entityID] |= COMPONENT_PHYSICS;

		// Create main body
		b2BodyDef bodyDef;
		bodyDef.type = type;
		bodyDef.awake = IsActive(entityID);
		bodyDef.position = def.position;
		bodyDef.angle = def.angle;
		bodyDef.linearVelocity = def.velocity;
		bodyDef.angularVelocity = def.angularVelocity;
		bodyDef.fixedRotation = fixedRotation;
		bodyDef.bullet = continuousCollisionDetection;
		m_physicsComponents[entityID].mainBody.entityID = entityID;
		m_physicsComponents[entityID].mainBody.isClone = false;
		SetB2BodyPtr(&m_physicsComponents[entityID].mainBody, m_b2WorldPtr->CreateBody(&bodyDef));

		// Create clones
		CloneSectionList cloneLocationList{ GetCloneSectionList(*m_physicsComponents[entityID].mainBody.b2BodyPtr) };
		unsigned i{ 0 };
		for(CloneBody& cloneBody : m_physicsComponents[entityID].cloneBodyList)
		{
			// Set clone body
			cloneBody.entityID = entityID;
			cloneBody.isClone = true;
			cloneBody.cloneIndex = i;
			cloneBody.section = cloneLocationList.at(i);

			// Create clone body
			bodyDef.position = def.position + GetCloneOffset(cloneLocationList.at(i));
			SetB2BodyPtr(&cloneBody, m_b2WorldPtr->CreateBody(&bodyDef));

			// Traverse clone location list at the same time
			++i;
		}
		m_physicsWrapDatas[entityID].crossedLeftBound = false;
		m_physicsWrapDatas[entityID].crossedRightBound = false;
		m_physicsWrapDatas[entityID].crossedLowerBound = false;
		m_physicsWrapDatas[entityID].crossedUpperBound = false;
		m_physicsWrapDatas[entityID].requiresManualWrapping = false;
		m_physicsComponents[entityID].disableCollisions = false;

		// LoadXML saved states
		m_lastTransforms[entityID] = m_physicsComponents[entityID].mainBody.b2BodyPtr->GetTransform();
		m_smoothedTransforms[entityID] = m_lastTransforms[entityID];
		m_lastLinearVelocities[entityID] = m_physicsComponents[entityID].mainBody.b2BodyPtr->GetLinearVelocity();
	}
	void World::AddCircleShape(WorldID entityID, const d2d::Material& material, const d2d::Filter& filter,
		float sizeRelativeToWidth, const b2Vec2& position, bool isSensor)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		if(HasPhysics(entityID) && HasSize2D(entityID))
		{
			// Add to main body
			float size{ sizeRelativeToWidth * m_sizeComponents[entityID].x };
			m_shapeFactory.AddCircleShape(*m_physicsComponents[entityID].mainBody.b2BodyPtr, size,
				material, filter, isSensor, position);

			// Add to clones
			for(unsigned i = 0; i < WORLD_NUM_CLONES; ++i)
				m_shapeFactory.AddCircleShape(*m_physicsComponents[entityID].cloneBodyList[i].b2BodyPtr, size,
					material, filter, isSensor, position);
		}
	}
	void World::AddRectShape(WorldID entityID, const d2d::Material& material, const d2d::Filter& filter,
		const b2Vec2& relativeSize, bool isSensor, const b2Vec2& position, float angle)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		if(HasPhysics(entityID) && HasSize2D(entityID))
		{
			// Add to main body
			b2Vec2 size{ m_sizeComponents[entityID].x * relativeSize.x, m_sizeComponents[entityID].y * relativeSize.y };
			m_shapeFactory.AddRectShape(*m_physicsComponents[entityID].mainBody.b2BodyPtr, size,
				material, filter, isSensor, position, angle);

			// Add to clones
			for(unsigned i = 0; i < WORLD_NUM_CLONES; ++i)
				m_shapeFactory.AddRectShape(*m_physicsComponents[entityID].cloneBodyList[i].b2BodyPtr, size,
					material, filter, isSensor, position, angle);
		}
	}
	void World::AddShapes(WorldID entityID, const std::string& model,
		const d2d::Material& material, const d2d::Filter& filter, bool isSensor,
		const b2Vec2& position, float angle)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		if(HasPhysics(entityID) && HasSize2D(entityID))
		{
			// Add to main body
			m_shapeFactory.AddShapes(*m_physicsComponents[entityID].mainBody.b2BodyPtr, m_sizeComponents[entityID], model,
				material, filter, isSensor, position, angle);

			// Add to clones
			for(unsigned i = 0; i < WORLD_NUM_CLONES; ++i)
				m_shapeFactory.AddShapes(*m_physicsComponents[entityID].cloneBodyList[i].b2BodyPtr, m_sizeComponents[entityID], model,
					material, filter, isSensor, position, angle);
		}
	}
	//+------------------------\----------------------------------
	//|	  Visual Components    |
	//\------------------------/----------------------------------
	void World::AddDrawRadarComponent(WorldID entityID, const DrawRadarComponent& radarComponent)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_componentBits[entityID] |= COMPONENT_DRAW_ON_RADAR;
		m_drawRadarComponents[entityID] = radarComponent;
	}
	void World::AddDrawFixturesComponent(WorldID entityID, const DrawFixturesComponent& fixturesComponent)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_componentBits[entityID] |= COMPONENT_DRAW_FIXTURES;
		m_drawFixtureComponents[entityID] = fixturesComponent;
	}
	void World::AddDrawAnimationComponent(WorldID entityID, const d2d::AnimationDef& animationDef)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_componentBits[entityID] |= COMPONENT_DRAW_ANIMATION;
		m_drawAnimationComponents[entityID].animation.Init(animationDef);
	}
	void World::SetAnimationLayer(WorldID entityID, int layer)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		d2d::Clamp(layer, m_settings.drawLayerRange);
		m_drawAnimationComponents[entityID].layer = layer;
	}
	//+------------------------\----------------------------------
	//|		  Power-ups		   |
	//\------------------------/----------------------------------
	void World::AddPowerUpComponent(WorldID entityID, const PowerUpComponent& powerUp)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_componentBits[entityID] |= COMPONENT_POWERUP;
		m_powerUpComponents[entityID] = powerUp;
	}
	void World::AddIconCollectorComponent(WorldID entityID, unsigned iconsCollected)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_componentBits[entityID] |= COMPONENT_ICON_COLLECTOR;
		m_iconCollectorComponents[entityID].iconsCollected = iconsCollected;
	}
	//+--------------------\--------------------------------------
	//|	  Life and Death   |
	//\--------------------/--------------------------------------
	void World::AddHealthComponent(WorldID entityID, float maxHP)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_componentBits[entityID] |= COMPONENT_HEALTH;
		m_healthComponents[entityID].hpMax = maxHP;
		m_healthComponents[entityID].hp = maxHP;
		m_healthComponents[entityID].deathDamage = 0.0f;
	}
	void World::AddParentComponent(WorldID entityID, WorldID parentID)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_componentBits[entityID] |= COMPONENT_PARENT;
		m_parentComponents[entityID] = parentID;
	}
	void World::AddParticleExplosionOnDeathComponent(WorldID entityID, float relativeSize,
		unsigned numParticles, const d2d::Range<float>& speedRange, float damageBasedSpeedIncreaseFactor,
		const d2d::Range<int>& sizeIndexRange, const d2d::ColorRange& colorRange,
		float lifetime, float fadeIn, float fadeOut)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_componentBits[entityID] |= COMPONENT_PARTICLE_EXPLOSION;
		m_particleExplosionComponents[entityID].relativeSize = relativeSize;
		m_particleExplosionComponents[entityID].numParticles = numParticles;
		m_particleExplosionComponents[entityID].speedRange = speedRange;
		m_particleExplosionComponents[entityID].damageBasedSpeedIncreaseFactor = damageBasedSpeedIncreaseFactor;
		m_particleExplosionComponents[entityID].sizeIndexRange = sizeIndexRange;
		m_particleExplosionComponents[entityID].colorRange = colorRange;
		m_particleExplosionComponents[entityID].lifetime = lifetime;
		m_particleExplosionComponents[entityID].fadeIn = fadeIn;
		m_particleExplosionComponents[entityID].fadeOut = fadeOut;
	}
	void World::AddDestructionDelayComponent(WorldID entityID, float delay)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_componentBits[entityID] |= COMPONENT_DESTRUCTION_DELAY;
		m_destructionDelayComponents[entityID] = std::max(delay, 0.0f);
	}
	void World::AddDestructionDelayOnContactComponent(WorldID entityID, float delay)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_componentBits[entityID] |= COMPONENT_DESTRUCTION_DELAY_ON_CONTACT;
		m_destructionDelayOnContactComponents[entityID] = std::max(delay, 0.0f);
	}
	void World::AddDestructionChanceOnContactComponent(WorldID entityID, float destructionChance)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_componentBits[entityID] |= COMPONENT_DESTRUCTION_CHANCE_ON_CONTACT;
		m_destructionChanceOnContactComponents[entityID] = std::clamp(destructionChance, 0.0f, 1.0f);
	}
	//+---------------------------\-------------------------------
	//|	  Projectile Launchers    |
	//\---------------------------/-------------------------------
	void World::AddProjectileLauncherComponent(WorldID entityID, unsigned numSlots, bool secondaryLaunchers)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		d2Assert(numSlots <= WORLD_MAX_PROJECTILE_LAUNCHER_SLOTS);
		m_componentBits[entityID] |= (secondaryLaunchers ? COMPONENT_SECONDARY_PROJECTILE_LAUNCHER : COMPONENT_PRIMARY_PROJECTILE_LAUNCHER);

		ProjectileLauncherComponent* launcherComponentPtr{
			secondaryLaunchers ? &m_secondaryProjectileLauncherComponents[entityID] : &m_primaryProjectileLauncherComponents[entityID] };

		launcherComponentPtr->factor = 0.0f;
		launcherComponentPtr->numSlots = numSlots;
		for(unsigned i = 0; i < WORLD_MAX_PROJECTILE_LAUNCHER_SLOTS; ++i)
			launcherComponentPtr->projectileLaunchers[i].enabled = false;
	}
	void World::AddProjectileLauncher(WorldID entityID, unsigned slot, const ProjectileDef& projectileDef,
		const b2Vec2& localRelativePosition, float impulse, float interval, bool temporarilyDisabled, bool secondaryLaunchers)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		if(IsValidProjectileLauncherSlot(entityID, slot, secondaryLaunchers))
		{
			ProjectileLauncherComponent* launcherComponentPtr{
				secondaryLaunchers ? &m_secondaryProjectileLauncherComponents[entityID] : &m_primaryProjectileLauncherComponents[entityID] };
			launcherComponentPtr->projectileLaunchers[slot].enabled = true;
			launcherComponentPtr->projectileLaunchers[slot].temporarilyDisabled = temporarilyDisabled;
			launcherComponentPtr->projectileLaunchers[slot].projectileDef = projectileDef;
			launcherComponentPtr->projectileLaunchers[slot].localRelativePosition = localRelativePosition;
			launcherComponentPtr->projectileLaunchers[slot].impulse = impulse;
			launcherComponentPtr->projectileLaunchers[slot].interval = interval;
			launcherComponentPtr->projectileLaunchers[slot].intervalAccumulator = interval;
		}
	}
	void World::RemoveProjectileLauncher(WorldID entityID, unsigned slot, bool secondaryLaunchers)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		if(IsValidProjectileLauncherSlot(entityID, slot, secondaryLaunchers))
		{
			ProjectileLauncherComponent* launcherComponentPtr{
				secondaryLaunchers ? &m_secondaryProjectileLauncherComponents[entityID] : &m_primaryProjectileLauncherComponents[entityID] };
			launcherComponentPtr->projectileLaunchers[slot].enabled = false;
		}
	}
	bool World::IsValidProjectileLauncherSlot(WorldID entityID, unsigned slot, bool secondaryLaunchers) const
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		const ProjectileLauncherComponent* launcherComponentPtr{
			secondaryLaunchers ? &m_secondaryProjectileLauncherComponents[entityID] : &m_primaryProjectileLauncherComponents[entityID] };
		ComponentBits component{ secondaryLaunchers ? COMPONENT_SECONDARY_PROJECTILE_LAUNCHER : COMPONENT_PRIMARY_PROJECTILE_LAUNCHER };
		return HasComponents(entityID, component) && slot < launcherComponentPtr->numSlots;
	}

	//+----------------------\------------------------------------
	//|	   Getting around	 |
	//\----------------------/------------------------------------
	void World::AddThrusterComponent(WorldID entityID, unsigned numSlots, float initialFactor)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		d2Assert(numSlots <= WORLD_MAX_THRUSTER_SLOTS);
		m_componentBits[entityID] |= COMPONENT_THRUSTER;

		ThrusterComponent& thrusterComponent{ m_thrusterComponents[entityID] };
		thrusterComponent.factor = initialFactor;
		thrusterComponent.numSlots = numSlots;
		for(unsigned i = 0; i < WORLD_MAX_THRUSTER_SLOTS; ++i)
			thrusterComponent.thrusters[i].enabled = false;
	}
	void World::AddThruster(WorldID entityID, unsigned slot, const d2d::AnimationDef& animationDef,
		float acceleration, float fuelPerSecond, const b2Vec2& localRelativePosition)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		d2Assert(IsValidThrusterSlot(entityID, slot));
		m_thrusterComponents[entityID].thrusters[slot].enabled = true;
		m_thrusterComponents[entityID].thrusters[slot].animation.Init(animationDef);
		m_thrusterComponents[entityID].thrusters[slot].acceleration = acceleration;
		m_thrusterComponents[entityID].thrusters[slot].localRelativePosition = localRelativePosition;
		m_thrusterComponents[entityID].thrusters[slot].fuelPerSecond = fuelPerSecond;
	}
	void World::RemoveThruster(WorldID entityID, unsigned slot)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		d2Assert(IsValidThrusterSlot(entityID, slot));
		m_primaryProjectileLauncherComponents[entityID].projectileLaunchers[slot].enabled = false;
	}
	bool World::IsValidThrusterSlot(WorldID entityID, unsigned slot) const
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		return (HasComponents(entityID, COMPONENT_THRUSTER) &&
			slot < m_thrusterComponents[entityID].numSlots);
	}
	void World::AddSetThrustFactorAfterDelayComponent(WorldID entityID, float thrustFactor, float delay)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_componentBits[entityID] |= COMPONENT_SET_THRUST_AFTER_DELAY;
		m_setThrustFactorAfterDelayComponents[entityID].factor = thrustFactor;
		m_setThrustFactorAfterDelayComponents[entityID].delay = delay;
	}

	void World::AddBoosterComponent(WorldID entityID, float factor, float boostSeconds, float cooldownSeconds)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_componentBits[entityID] |= COMPONENT_BOOSTER;
		m_boosterComponents[entityID].factor = factor;
		m_boosterComponents[entityID].boostSeconds = boostSeconds;
		m_boosterComponents[entityID].cooldownSeconds = cooldownSeconds;

		m_boosterComponents[entityID].engaged = false;
		m_boosterComponents[entityID].secondsLeft = 0.0f;
		m_boosterComponents[entityID].cooldownSecondsLeft = 0.0f;
	}
	void World::AddFuelComponent(WorldID entityID, float level, float max)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_componentBits[entityID] |= COMPONENT_FUEL;
		m_fuelComponents[entityID].max = max;
		m_fuelComponents[entityID].level = level;
	}
	void World::AddRotatorComponent(WorldID entityID, float rotationSpeed)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_componentBits[entityID] |= COMPONENT_ROTATOR;
		m_rotatorComponents[entityID].factor = 0.0f;
		m_rotatorComponents[entityID].lastFactor = 0.0f;
		m_rotatorComponents[entityID].rotationSpeed = rotationSpeed;
	}
	void World::AddBrakeComponent(WorldID entityID, float deceleration)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_componentBits[entityID] |= COMPONENT_BRAKE;
		m_brakeComponents[entityID].factor = 0.0f;
		m_brakeComponents[entityID].deceleration = deceleration;
	}
}
