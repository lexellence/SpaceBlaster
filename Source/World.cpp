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
		m_destroyBuffer.clear();

		// Clear components and flags
		for(EntityID i = 0; i < WORLD_MAX_ENTITIES; ++i)
			m_componentBits[i].reset();
		for(EntityID i = 0; i < WORLD_MAX_ENTITIES; ++i)
			m_flagBits[i].reset();

		// Create new Box2D physics world
		m_b2WorldPtr = new b2World{ b2Vec2_zero };
		m_b2WorldPtr->SetAutoClearForces(false);
		m_b2WorldPtr->SetContactListener(this);
		m_b2WorldPtr->SetContactFilter(this);
		m_b2WorldPtr->SetDestructionListener(this);

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
	void World::SetProjectileLauncherListener(ProjectileLauncherListener* listenerPtr)
	{
		m_projectileLauncherListenerPtr = listenerPtr;
	}
	void World::SetExitListener(ExitListener* listenerPtr)
	{
		m_exitListenerPtr = listenerPtr;
	}
	//+------------------------\----------------------------------
	//|	  Creating Entities    |
	//\------------------------/----------------------------------
	EntityID World::NewEntityID(const b2Vec2& size, int drawLayer, bool activate)
	{
		for(EntityID entityID = 0; entityID < WORLD_MAX_ENTITIES; ++entityID)
			if(m_componentBits[entityID].none() && m_flagBits[entityID].none())
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
	void World::Destroy(EntityID id)
	{
		m_destroyBuffer.insert(id);
	}
	void World::SetFlag(EntityID entityID, FlagBit flagBit, bool enable)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		if(enable)
			m_flagBits[entityID].set(flagBit);
		else
			m_flagBits[entityID].reset(flagBit);
	}
	void World::SetFlagSet(EntityID entityID, FlagBitset flagBits, bool enable)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		if(enable)
			m_flagBits[entityID] |= flagBits;
		else
			m_flagBits[entityID] &= ~flagBits;
	}
	void World::RemoveAllFlags(EntityID entityID)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_flagBits[entityID].reset();
	}
	void World::RemoveComponent(EntityID entityID, ComponentBit componentBit)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		if(componentBit == COMPONENT_PHYSICS)
			DestroyB2Bodies(entityID);
		m_componentBits[entityID].reset(componentBit);
	}
	void World::RemoveComponentSet(EntityID entityID, ComponentBitset componentBits)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		if(componentBits[COMPONENT_PHYSICS])
			DestroyB2Bodies(entityID);
		m_componentBits[entityID] &= ~componentBits;
	}
	void World::RemoveAllComponents(EntityID entityID)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		DestroyB2Bodies(entityID);
		m_componentBits[entityID].reset();
	}
	void World::DestroyB2Bodies(EntityID entityID)
	{
		if(HasPhysics(entityID))
		{
			m_b2WorldPtr->DestroyBody(m_physicsComponents[entityID].mainBody.b2BodyPtr);
			m_physicsComponents[entityID].mainBody.b2BodyPtr = nullptr;
			for(CloneBody& cloneBody : m_physicsComponents[entityID].cloneBodyList)
			{
				m_b2WorldPtr->DestroyBody(cloneBody.b2BodyPtr);
				cloneBody.b2BodyPtr = nullptr;
			}
		}
	}
	//void World::RemoveAllComponentsExcept(EntityID entityID, BitMask componentBits)
	//{
	//	d2Assert(entityID < WORLD_MAX_ENTITIES);
	//	m_componentBits[entityID].reset();
	//	m_componentBits[entityID] |= componentBits;
	//}
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
			auto closestEntities = GetClosestEntities(position, newBoundingRadius, 1);
			if(closestEntities.empty() || closestEntities.front().second >= minGap)
				acceptablePositionFound = true;
		} while(!acceptablePositionFound && ++attempts < maxAttempts);

		if(acceptablePositionFound)
			positionOut = position;

		return acceptablePositionFound;
	}
	void World::AddPhysicsComponent(EntityID entityID, b2BodyType type,
		const InstanceDef& def, bool fixedRotation, bool continuousCollisionDetection)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_componentBits[entityID].set(COMPONENT_PHYSICS);

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
	std::vector<b2Fixture*> World::AddCircleShape(EntityID entityID, const d2d::Material& material, const d2d::Filter& filter,
		float sizeRelativeToWidth, const b2Vec2& position, bool isSensor)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		std::vector<b2Fixture*> fixturePtrList;
		if(HasPhysics(entityID) && HasSize2D(entityID))
		{
			// Add to main body
			float size{ sizeRelativeToWidth * m_sizeComponents[entityID].x };
			b2Fixture* fixturePtr = m_shapeFactory.AddCircleShape(*m_physicsComponents[entityID].mainBody.b2BodyPtr, 
				size, material, filter, isSensor, position);
			fixturePtrList.push_back(fixturePtr);

			// Add to clones
			for(unsigned i = 0; i < WORLD_NUM_CLONES; ++i)
			{
				fixturePtr = m_shapeFactory.AddCircleShape(*m_physicsComponents[entityID].cloneBodyList[i].b2BodyPtr, 
					size, material, filter, isSensor, position);
				fixturePtrList.push_back(fixturePtr);
			}
		}
		return fixturePtrList;
	}
	std::vector<b2Fixture*> World::AddRectShape(EntityID entityID, const d2d::Material& material, const d2d::Filter& filter,
		const b2Vec2& relativeSize, bool isSensor, const b2Vec2& position, float angle)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		std::vector<b2Fixture*> fixturePtrList;
		if(HasPhysics(entityID) && HasSize2D(entityID))
		{
			// Add to main body
			b2Vec2 size{ m_sizeComponents[entityID].x * relativeSize.x, m_sizeComponents[entityID].y * relativeSize.y };
			b2Fixture* fixturePtr = m_shapeFactory.AddRectShape(*m_physicsComponents[entityID].mainBody.b2BodyPtr, 
				size, material, filter, isSensor, position, angle);
			fixturePtrList.push_back(fixturePtr);

			// Add to clones
			for(unsigned i = 0; i < WORLD_NUM_CLONES; ++i)
			{
				fixturePtr = m_shapeFactory.AddRectShape(*m_physicsComponents[entityID].cloneBodyList[i].b2BodyPtr, size,
					material, filter, isSensor, position, angle);
				fixturePtrList.push_back(fixturePtr);
			}
		}
		return fixturePtrList;
	}
	std::vector<b2Fixture*> World::AddShapes(EntityID entityID, const std::string& model,
		const d2d::Material& material, const d2d::Filter& filter, bool isSensor,
		const b2Vec2& position, float angle)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		std::vector<b2Fixture*> fixturePtrList;
		if(HasPhysics(entityID) && HasSize2D(entityID))
		{
			// Add to main body
			fixturePtrList = m_shapeFactory.AddShapes(*m_physicsComponents[entityID].mainBody.b2BodyPtr, m_sizeComponents[entityID], model,
				material, filter, isSensor, position, angle);

			// Add to clones
			for(unsigned i = 0; i < WORLD_NUM_CLONES; ++i)
			{
				std::vector<b2Fixture*> cloneFixturePtrList;
				m_shapeFactory.AddShapes(*m_physicsComponents[entityID].cloneBodyList[i].b2BodyPtr, m_sizeComponents[entityID], model,
					material, filter, isSensor, position, angle);
				fixturePtrList.insert(std::end(fixturePtrList), std::begin(cloneFixturePtrList), std::end(cloneFixturePtrList));
			}
		}
		return fixturePtrList;
	}
	//+------------------------\----------------------------------
	//|	  Visual Components    |
	//\------------------------/----------------------------------
	void World::AddDrawRadarComponent(EntityID entityID, const DrawRadarComponent& radarComponent)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_componentBits[entityID].set(COMPONENT_DRAW_ON_RADAR);
		m_drawRadarComponents[entityID] = radarComponent;
	}
	void World::AddDrawFixturesComponent(EntityID entityID, const DrawFixturesComponent& fixturesComponent)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_componentBits[entityID].set(COMPONENT_DRAW_FIXTURES);
		m_drawFixtureComponents[entityID] = fixturesComponent;
	}
	void World::AddDrawAnimationComponent(EntityID entityID, const d2d::AnimationDef& animationDef)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_componentBits[entityID].set(COMPONENT_DRAW_ANIMATION);
		m_drawAnimationComponents[entityID].animation.Init(animationDef);
	}
	void World::SetAnimationLayer(EntityID entityID, int layer)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		d2d::Clamp(layer, m_settings.drawLayerRange);
		m_drawAnimationComponents[entityID].layer = layer;
	}
	//+------------------------\----------------------------------
	//|		  Power-ups		   |
	//\------------------------/----------------------------------
	void World::AddPowerUpComponent(EntityID entityID, const PowerUpComponent& powerUp)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_componentBits[entityID].set(COMPONENT_POWERUP);
		m_powerUpComponents[entityID] = powerUp;
	}
	void World::AddIconCollectorComponent(EntityID entityID, float* creditsPtr)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_componentBits[entityID].set(COMPONENT_ICON_COLLECTOR);
		m_iconCollectorComponents[entityID].creditsPtr = creditsPtr;
	}
	//+--------------------\--------------------------------------
	//|	  Life and Death   |
	//\--------------------/--------------------------------------
	void World::AddHealthComponent(EntityID entityID, float maxHP)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_componentBits[entityID].set(COMPONENT_HEALTH);
		m_healthComponents[entityID].hpMax = maxHP;
		m_healthComponents[entityID].hp = maxHP;
		m_healthComponents[entityID].deathDamage = 0.0f;
	}
	void World::AddParentComponent(EntityID entityID, EntityID parentID)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_componentBits[entityID].set(COMPONENT_PARENT);
		m_parentComponents[entityID] = parentID;
	}
	void World::AddParticleExplosionOnDeathComponent(EntityID entityID, float relativeSize,
		unsigned numParticles, const d2d::Range<float>& speedRange, float damageBasedSpeedIncreaseFactor,
		const d2d::Range<int>& sizeIndexRange, const d2d::ColorRange& colorRange,
		float lifetime, float fadeIn, float fadeOut)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_componentBits[entityID].set(COMPONENT_PARTICLE_EXPLOSION);
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
	void World::AddDestructionDelayComponent(EntityID entityID, float delay)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_componentBits[entityID].set(COMPONENT_DESTRUCTION_DELAY);
		m_destructionDelayComponents[entityID] = std::max(delay, 0.0f);
	}
	void World::AddDestructionDelayOnContactComponent(EntityID entityID, float delay)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_componentBits[entityID].set(COMPONENT_DESTRUCTION_DELAY_ON_CONTACT);
		m_destructionDelayOnContactComponents[entityID] = std::max(delay, 0.0f);
	}
	void World::AddDestructionChanceOnContactComponent(EntityID entityID, float destructionChance)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_componentBits[entityID].set(COMPONENT_DESTRUCTION_CHANCE_ON_CONTACT);
		m_destructionChanceOnContactComponents[entityID] = std::clamp(destructionChance, 0.0f, 1.0f);
	}
	//+---------------------------\-------------------------------
	//|	  Projectile Launchers    |
	//\---------------------------/-------------------------------
	void World::AddProjectileLauncherComponent(EntityID entityID, unsigned numSlots, bool secondaryLaunchers)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		d2Assert(numSlots <= WORLD_MAX_PROJECTILE_LAUNCHER_SLOTS);
		m_componentBits[entityID].set(secondaryLaunchers ? COMPONENT_SECONDARY_PROJECTILE_LAUNCHER : COMPONENT_PRIMARY_PROJECTILE_LAUNCHER);

		ProjectileLauncherComponent* launcherComponentPtr{
			secondaryLaunchers ? &m_secondaryProjectileLauncherComponents[entityID] : &m_primaryProjectileLauncherComponents[entityID] };

		launcherComponentPtr->factor = 0.0f;
		launcherComponentPtr->numSlots = numSlots;
		for(unsigned i = 0; i < WORLD_MAX_PROJECTILE_LAUNCHER_SLOTS; ++i)
			launcherComponentPtr->projectileLaunchers[i].enabled = false;
	}
	void World::AddProjectileLauncher(EntityID entityID, unsigned slot, const ProjectileDef& projectileDef,
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
	void World::RemoveProjectileLauncher(EntityID entityID, unsigned slot, bool secondaryLaunchers)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		if(IsValidProjectileLauncherSlot(entityID, slot, secondaryLaunchers))
		{
			ProjectileLauncherComponent* launcherComponentPtr{
				secondaryLaunchers ? &m_secondaryProjectileLauncherComponents[entityID] : &m_primaryProjectileLauncherComponents[entityID] };
			launcherComponentPtr->projectileLaunchers[slot].enabled = false;
		}
	}
	bool World::IsValidProjectileLauncherSlot(EntityID entityID, unsigned slot, bool secondaryLaunchers) const
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		const ProjectileLauncherComponent* launcherComponentPtr{
			secondaryLaunchers ? &m_secondaryProjectileLauncherComponents[entityID] : &m_primaryProjectileLauncherComponents[entityID] };
		ComponentBit component{ secondaryLaunchers ? COMPONENT_SECONDARY_PROJECTILE_LAUNCHER : COMPONENT_PRIMARY_PROJECTILE_LAUNCHER };
		return HasComponent(entityID, component) && slot < launcherComponentPtr->numSlots;
	}
	//+--------------------------\--------------------------------
	//|	    LaunchProjectile     |
	//\--------------------------/--------------------------------
	EntityID World::LaunchProjectile(const ProjectileDef& projectileDef, const b2Vec2& position,
		float angle, float impulse, const b2Vec2& parentVelocity, EntityID parentID)
	{
		EntityID id{ NewEntityID(projectileDef.dimensions, GetDrawLayer(parentID) - 1, true) };
		InstanceDef def;
		def.position = position;
		def.angle = angle;
		def.velocity = parentVelocity;
		def.angularVelocity = 0.0f;
		AddPhysicsComponent(id, b2_dynamicBody, def,
			projectileDef.fixedRotation, projectileDef.continuousCollisionDetection);

		AddShapes(id, projectileDef.model.name, projectileDef.material, projectileDef.filter);

		if(impulse > 0.0f)
		{
			b2Vec2 unitDirectionVector{ cos(angle), sin(angle) };
			ApplyLinearImpulseToCenter(id, impulse * unitDirectionVector);
		}

		AddDrawAnimationComponent(id, projectileDef.model.animationDef);

		if(projectileDef.destructionDelay)
			AddDestructionDelayComponent(id, projectileDef.destructionDelayTime);

		if(projectileDef.destructionDelayOnContact)
			AddDestructionDelayOnContactComponent(id, projectileDef.destructionDelayOnContactTime);

		if(projectileDef.destructionChanceOnContact)
			AddDestructionChanceOnContactComponent(id, projectileDef.destructionChance);

		AddParentComponent(id, parentID);
		if(projectileDef.ignoreParentCollisionsUntilFirstContactEnd)
			SetFlag(id, FLAG_IGNORE_PARENT_COLLISIONS_UNTIL_FIRST_CONTACT_END, true);

		if(projectileDef.acceleration > 0.0f && projectileDef.accelerationTime > 0.0f)
		{
			AddThrusterComponent(id, 1, 1.0f);
			AddThruster(id, 0, {}, projectileDef.acceleration, 0.0f, b2Vec2_zero);
			AddSetThrustFactorAfterDelayComponent(id, 0.0f, projectileDef.accelerationTime);
		}

		// Notify listener
		if(m_projectileLauncherListenerPtr)
			m_projectileLauncherListenerPtr->ProjectileLaunched(projectileDef, parentID);

		return id;
	}
	//+----------------------\------------------------------------
	//|	   Getting around	 |
	//\----------------------/------------------------------------
	void World::AddThrusterComponent(EntityID entityID, unsigned numSlots, float initialFactor)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		d2Assert(numSlots <= WORLD_MAX_THRUSTER_SLOTS);
		m_componentBits[entityID].set(COMPONENT_THRUSTER);

		ThrusterComponent& thrusterComponent{ m_thrusterComponents[entityID] };
		thrusterComponent.factor = initialFactor;
		thrusterComponent.numSlots = numSlots;
		for(unsigned i = 0; i < WORLD_MAX_THRUSTER_SLOTS; ++i)
			thrusterComponent.thrusters[i].enabled = false;
	}
	void World::AddThruster(EntityID entityID, unsigned slot, const d2d::AnimationDef& animationDef,
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
	void World::RemoveThruster(EntityID entityID, unsigned slot)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		d2Assert(IsValidThrusterSlot(entityID, slot));
		m_primaryProjectileLauncherComponents[entityID].projectileLaunchers[slot].enabled = false;
	}
	bool World::IsValidThrusterSlot(EntityID entityID, unsigned slot) const
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		return (HasComponent(entityID, COMPONENT_THRUSTER) &&
			slot < m_thrusterComponents[entityID].numSlots);
	}
	void World::AddSetThrustFactorAfterDelayComponent(EntityID entityID, float thrustFactor, float delay)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_componentBits[entityID].set(COMPONENT_SET_THRUST_AFTER_DELAY);
		m_setThrustFactorAfterDelayComponents[entityID].factor = thrustFactor;
		m_setThrustFactorAfterDelayComponents[entityID].delay = delay;
	}

	void World::AddBoosterComponent(EntityID entityID, float factor, float boostSeconds, float cooldownSeconds)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_componentBits[entityID].set(COMPONENT_BOOSTER);
		m_boosterComponents[entityID].factor = factor;
		m_boosterComponents[entityID].boostSeconds = boostSeconds;
		m_boosterComponents[entityID].cooldownSeconds = cooldownSeconds;

		m_boosterComponents[entityID].engaged = false;
		m_boosterComponents[entityID].secondsLeft = 0.0f;
		m_boosterComponents[entityID].cooldownSecondsLeft = 0.0f;
	}
	void World::AddFuelComponent(EntityID entityID, float level, float max)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_componentBits[entityID].set(COMPONENT_FUEL);
		m_fuelComponents[entityID].max = max;
		m_fuelComponents[entityID].level = level;
	}
	void World::AddRotatorComponent(EntityID entityID, float rotationSpeed)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_componentBits[entityID].set(COMPONENT_ROTATOR);
		m_rotatorComponents[entityID].factor = 0.0f;
		m_rotatorComponents[entityID].lastFactor = 0.0f;
		m_rotatorComponents[entityID].rotationSpeed = rotationSpeed;
	}
	void World::AddBrakeComponent(EntityID entityID, float deceleration)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		m_componentBits[entityID].set(COMPONENT_BRAKE);
		m_brakeComponents[entityID].factor = 0.0f;
		m_brakeComponents[entityID].deceleration = deceleration;
	}
	void World::AddRadarComponent(EntityID entityID, float range)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		if(HasPhysics(entityID))
		{
			m_componentBits[entityID].set(COMPONENT_RADAR);
			m_radarComponents[entityID].range = range;
			m_radarComponents[entityID].bodiesInRange.clear();
			CreateRadarFixture(entityID);
		}
	}
	void World::CreateRadarFixture(EntityID entityID)
	{
		// Add to main body
		b2Fixture* b2FixturePtr = m_shapeFactory.AddCircleShape(
			*m_physicsComponents[entityID].mainBody.b2BodyPtr,
			m_radarComponents[entityID].range * 2.0f, {}, {}, true, b2Vec2_zero);
		b2FixturePtr->GetUserData().isRadar = true;
		m_radarComponents[entityID].b2FixturePtr = b2FixturePtr;
	}
	void World::MoveRadarToMainBody(EntityID entityID)
	{
		d2Assert(entityID < WORLD_MAX_ENTITIES);
		if(HasPhysics(entityID) && HasComponent(entityID, COMPONENT_RADAR))
		{
			Body* bodyPtr = GetUserBodyFromFixture(m_radarComponents[entityID].b2FixturePtr);
			if(bodyPtr->b2BodyPtr != m_physicsComponents[entityID].mainBody.b2BodyPtr)
			{
				bodyPtr->b2BodyPtr->DestroyFixture(m_radarComponents[entityID].b2FixturePtr);
				m_radarComponents[entityID].b2FixturePtr = nullptr;
				CreateRadarFixture(entityID);
			}
		}
	}
}
