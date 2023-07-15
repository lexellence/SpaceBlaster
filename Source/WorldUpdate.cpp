/**************************************************************************************\
** File: WorldUpdate.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the update functions of the World class
**
\**************************************************************************************/
#include "pch.h"
#include "World.h"
#include "ParticleSystem.h"
#include <iomanip>

namespace Space
{
	//+-------------\---------------------------------------------
	//|	  Update    |
	//\-------------/---------------------------------------------
	void World::Update(float dt, PlayerController& playerController)
	{
		// Slow-mo rather than allowing a huge leap forward without user input
		d2d::ClampHigh(dt, m_settings.maxUpdateTime);

		// Add time to internal buffer
		m_timestepAccumulator += dt;

		// Calculate the number of physics steps to take
		float stepTime{ 1.0f / m_settings.stepsPerSecond };
		int numSteps{ (int)std::floor(m_timestepAccumulator / stepTime) };
		d2Assert(numSteps >= 0);

		// Reduce the accumulator by the amount of time we are going to use
		if(numSteps > 0)
			m_timestepAccumulator -= numSteps * stepTime;

		// Do update in steps
		for(int i = 0; i < numSteps; ++i)
			SingleUpdateStep(stepTime, playerController);

		SmoothStates(m_timestepAccumulator / stepTime);
	}
	void World::SingleUpdateStep(float dt, PlayerController& playerController)
	{
		UpdateEntityCount();
		UpdateDestructionDelayComponents(dt);
		UpdatePlayerControllerComponents(dt, playerController);
		UpdateRotatorComponents();
		UpdateSetThrustFactorAfterDelayComponents(dt);
		UpdateThrusterComponents(dt);
		UpdateBoosterComponents(dt);
		UpdateBrakeComponents();
		UpdateProjectileLauncherComponents(dt, false);
		UpdateProjectileLauncherComponents(dt, true);
		UpdatePhysics(dt);
		m_particleSystem.Update(dt);
		UpdateDrawAnimationComponents(dt);
	}
	void World::UpdateEntityCount()
	{
		// Keep track of how close we get to running out of bodies
		m_highestActiveEntityCount = std::max(GetEntityCount(), m_highestActiveEntityCount);
	}
	void World::UpdateDestructionDelayComponents(float dt)
	{
		for(EntityID id = 0; id < WORLD_MAX_ENTITIES; ++id)
			if(HasComponent(id, COMPONENT_DESTRUCTION_DELAY) && IsActive(id))
				if(m_destructionDelayComponents[id] > 0.0f)
				{
					m_destructionDelayComponents[id] -= dt;
					if(m_destructionDelayComponents[id] <= 0.0f)
						Destroy(id);
				}
	}
	//+------------------------\----------------------------------
	//|	   Getting Around	   |
	//\------------------------/----------------------------------
	void World::UpdatePlayerControllerComponents(float dt, PlayerController& playerController)
	{
		for(EntityID id = 0; id < WORLD_MAX_ENTITIES; ++id)
			if(HasFlag(id, FLAG_PLAYER_CONTROLLED) && IsActive(id))
			{
				if(HasComponent(id, COMPONENT_PRIMARY_PROJECTILE_LAUNCHER))
					m_primaryProjectileLauncherComponents[id].factor = playerController.primaryFireFactor;
				if(HasComponent(id, COMPONENT_SECONDARY_PROJECTILE_LAUNCHER))
					m_secondaryProjectileLauncherComponents[id].factor = playerController.secondaryFireFactor;

				if(HasComponent(id, COMPONENT_ROTATOR))
				{
					m_rotatorComponents[id].lastFactor = m_rotatorComponents[id].factor;
					m_rotatorComponents[id].factor = playerController.turnFactor;
				}

				ThrusterComponent& thrusterComponent{ m_thrusterComponents[id] };
				if(HasComponent(id, COMPONENT_THRUSTER))
					thrusterComponent.factor = playerController.thrustFactor;
				if(playerController.boost && HasComponent(id, COMPONENT_BOOSTER) &&
					m_boosterComponents[id].secondsLeft <= 0.0f &&
					m_boosterComponents[id].cooldownSecondsLeft <= 0.0f)
				{
					m_boosterComponents[id].secondsLeft = m_boosterComponents[id].boostSeconds;
				}

				if(HasComponent(id, COMPONENT_BRAKE))
					m_brakeComponents[id].factor = playerController.brakeFactor;

			}
	}
	void World::UpdateRotatorComponents()
	{
		ComponentBitset requiredComponents;
		requiredComponents.set(COMPONENT_ROTATOR).set(COMPONENT_PHYSICS);
		for(EntityID id = 0; id < WORLD_MAX_ENTITIES; ++id)
			if(HasComponentSet(id, requiredComponents) && IsActive(id))
			{
				// If entity was just turning but now not, stop rotation.
				if(m_rotatorComponents[id].lastFactor != 0.0f && m_rotatorComponents[id].factor == 0.0f)
					m_physicsComponents[id].mainBody.b2BodyPtr->SetAngularVelocity(0.0f);

				// If entity is turning, tell it's body to rotate
				if(m_rotatorComponents[id].factor != 0.0f)
					m_physicsComponents[id].mainBody.b2BodyPtr->SetAngularVelocity(m_rotatorComponents[id].factor * m_rotatorComponents[id].rotationSpeed);
			}
	}
	void World::ApplyThrust(EntityID id, float acceleration)
	{
		float angle{ m_physicsComponents[id].mainBody.b2BodyPtr->GetAngle() };
		float inputAdjustedForce{ m_physicsComponents[id].mainBody.b2BodyPtr->GetMass() * acceleration * m_thrusterComponents[id].factor };
		b2Vec2 forceVec{ inputAdjustedForce * cosf(angle), inputAdjustedForce * sinf(angle) };
		ApplyForceToCenter(id, forceVec);
	}
	void World::UpdateThrusterComponents(float dt)
	{
		ComponentBitset requiredComponents;
		requiredComponents.set(COMPONENT_THRUSTER).set(COMPONENT_PHYSICS);
		for(EntityID id = 0; id < WORLD_MAX_ENTITIES; ++id)
			if(HasComponentSet(id, requiredComponents) && IsActive(id))
				if(m_thrusterComponents[id].factor > 0.0f)
				{
					d2Assert(m_thrusterComponents[id].numSlots <= WORLD_MAX_THRUSTER_SLOTS);
					float sumAccelerations = GetTotalThrusterAcceleration(id);
					float fuelRequired = GetTotalThrusterFuelRequired(id, dt);

					// Fuel
					if(fuelRequired > 0.0f && HasComponent(id, COMPONENT_FUEL))
					{
						if(m_fuelComponents[id].level < fuelRequired)
						{
							float fuelFactor = m_fuelComponents[id].level / fuelRequired;
							sumAccelerations *= fuelFactor;
							m_fuelComponents[id].level = 0.0f;
						}
						else
							m_fuelComponents[id].level -= fuelRequired;
					}

					// Thrust
					ApplyThrust(id, sumAccelerations);

					// Update thruster animations
					for(unsigned i = 0; i < m_thrusterComponents[id].numSlots; ++i)
						m_thrusterComponents[id].thrusters[i].animation.Update(dt);
				}
	}
	void World::UpdateSetThrustFactorAfterDelayComponents(float dt)
	{
		for(EntityID id = 0; id < WORLD_MAX_ENTITIES; ++id)
			if(HasComponent(id, COMPONENT_SET_THRUST_AFTER_DELAY) && IsActive(id))
			{
				m_setThrustFactorAfterDelayComponents[id].delay -= dt;
				if(m_setThrustFactorAfterDelayComponents[id].delay <= 0.0f)
				{
					if(HasComponent(id, COMPONENT_THRUSTER))
						m_thrusterComponents[id].factor = m_setThrustFactorAfterDelayComponents[id].factor;
					RemoveComponent(id, COMPONENT_SET_THRUST_AFTER_DELAY);
				}
			}
	}
	void World::UpdateBoosterComponents(float dt)
	{
		ComponentBitset requiredComponents;
		requiredComponents.set(COMPONENT_BOOSTER).set(COMPONENT_PHYSICS);
		for(EntityID id = 0; id < WORLD_MAX_ENTITIES; ++id)
			if(HasComponentSet(id, requiredComponents) && IsActive(id))
			{
				if(m_boosterComponents[id].secondsLeft > 0.0f)
				{
					// Boost mode
					m_boosterComponents[id].secondsLeft -= dt;
					if(m_boosterComponents[id].secondsLeft <= 0.0f)
					{
						// Switch to cooldown mode
						m_boosterComponents[id].cooldownSecondsLeft = m_boosterComponents[id].cooldownSeconds + m_boosterComponents[id].secondsLeft;
						m_boosterComponents[id].secondsLeft = 0.0f;
						d2d::ClampLow(m_boosterComponents[id].cooldownSecondsLeft, 0.0f);
					}
				}
				else if(m_boosterComponents[id].cooldownSecondsLeft > 0.0f)
				{
					// Cooldown mode
					m_boosterComponents[id].cooldownSecondsLeft -= dt;
					d2d::ClampLow(m_boosterComponents[id].cooldownSecondsLeft, 0.0f);
				}
			}
	}
	void World::UpdateBrakeComponents()
	{
		ComponentBitset requiredComponents;
		requiredComponents.set(COMPONENT_BRAKE).set(COMPONENT_PHYSICS);
		for(EntityID id = 0; id < WORLD_MAX_ENTITIES; ++id)
			if(HasComponentSet(id, requiredComponents) && IsActive(id))
				if(m_brakeComponents[id].factor > 0.0f)
				{
					b2Vec2 unitVelocity{ m_physicsComponents[id].mainBody.b2BodyPtr->GetLinearVelocity() };
					unitVelocity.Normalize();
					b2Vec2 brakeForce{ -m_brakeComponents[id].deceleration * m_physicsComponents[id].mainBody.b2BodyPtr->GetMass() * m_brakeComponents[id].factor * unitVelocity };
					ApplyForceToCenter(id, brakeForce);
				}
	}
	//+------------------------\----------------------------------
	//|	   Weapon Systems	   |
	//\------------------------/----------------------------------
	void World::UpdateProjectileLauncherComponents(float dt, bool secondaryLaunchers)
	{
		ComponentArray< ProjectileLauncherComponent >* projectileLauncherComponentsPtr;
		ComponentBitset requiredComponents;
		requiredComponents.set(COMPONENT_PHYSICS);
		if(secondaryLaunchers)
		{
			projectileLauncherComponentsPtr = &m_secondaryProjectileLauncherComponents;
			requiredComponents.set(COMPONENT_SECONDARY_PROJECTILE_LAUNCHER);
		}
		else
		{
			projectileLauncherComponentsPtr = &m_primaryProjectileLauncherComponents;
			requiredComponents.set(COMPONENT_PRIMARY_PROJECTILE_LAUNCHER);
		}
		ComponentArray< ProjectileLauncherComponent >& projectileLauncherComponents{ *projectileLauncherComponentsPtr };

		for(EntityID id = 0; id < WORLD_MAX_ENTITIES; ++id)
			if(HasComponentSet(id, requiredComponents) && IsActive(id))
			{
				for(unsigned i = 0; i < projectileLauncherComponents[id].numSlots; ++i)
				{
					ProjectileLauncher& launcher{ projectileLauncherComponents[id].projectileLaunchers[i] };
					if(launcher.intervalAccumulator < launcher.interval)
						launcher.intervalAccumulator += dt;
				}

				// If projectile system engaged, fire projectiles
				if(projectileLauncherComponents[id].factor > 0.0f)
					for(unsigned i = 0; i < projectileLauncherComponents[id].numSlots; ++i)
					{
						ProjectileLauncher& launcher{ projectileLauncherComponents[id].projectileLaunchers[i] };
						if(launcher.enabled && !launcher.temporarilyDisabled)
						{
							if(launcher.intervalAccumulator >= launcher.interval)
							{
								b2Body* b2BodyPtr{ m_physicsComponents[id].mainBody.b2BodyPtr };
								const b2Transform& transform{ b2BodyPtr->GetTransform() };
								b2Vec2 localBulletPosition{ launcher.localRelativePosition * m_sizeComponents[id] };
								b2Vec2 globalBulletPosition{ b2Mul(transform, localBulletPosition) };
								LaunchProjectile(launcher.projectileDef,
									globalBulletPosition, transform.q.GetAngle(), launcher.impulse,
									b2BodyPtr->GetLinearVelocity(), id);
								launcher.intervalAccumulator = 0.0f;
							}
						}
					}
			}
	}
	//+-------------\---------------------------------------------
	//|	Animations  |
	//\-------------/---------------------------------------------
	void World::UpdateDrawAnimationComponents(float dt)
	{
		for(EntityID id = 0; id < WORLD_MAX_ENTITIES; ++id)
			if(HasComponentSet(id, COMPONENT_DRAW_ANIMATION) && IsActive(id))
			{
				m_drawAnimationComponents[id].animation.Update(dt);
				if(!m_drawAnimationComponents[id].animation.IsAnimated() && HasFlag(id, FLAG_DESTRUCTION_ON_ANIMATION_COMPLETION))
					Destroy(id);
			}
	}
	//+-------------\---------------------------------------------
	//|	  Physics   |
	//\-------------/---------------------------------------------
	void World::UpdatePhysics(float dt)
	{
		ProcessDestroyBuffer();
		SaveVelocities();
		ResetSmoothStates();
		SyncClones();
		m_b2WorldPtr->Step(dt, m_settings.velocityIterationsPerStep, m_settings.positionIterationsPerStep);
		ProcessDestroyBuffer();
		SyncClones();
		WrapEntities();
		m_b2WorldPtr->ClearForces();
	}
	void World::SaveVelocities()
	{
		// Velocities saved for use in calculating particle explosion velocities
		for(EntityID id = 0; id < WORLD_MAX_ENTITIES; ++id)
			if(HasPhysics(id))
				m_lastLinearVelocities[id] = m_physicsComponents[id].mainBody.b2BodyPtr->GetLinearVelocity();
	}
	void World::EmptyPhysicsStep()
	{
		m_b2WorldPtr->Step(0.0f, 0, 0);
		ProcessDestroyBuffer();
	}
	void World::ResetSmoothStates()
	{
		for(EntityID id = 0; id < WORLD_MAX_ENTITIES; ++id)
			if(HasPhysics(id))
			{
				m_lastTransforms[id] = m_physicsComponents[id].mainBody.b2BodyPtr->GetTransform();
				m_smoothedTransforms[id] = m_lastTransforms[id];
			}
	}
	void World::SyncClones()
	{
		// For each entity, use quadrant to determine which clones it should have.
		// Then replace old clones with new ones at correct locations while retaining
		// existing ones which are already at the correct location.
		for(EntityID id = 0; id < WORLD_MAX_ENTITIES; ++id)
			if(HasPhysics(id) && IsActive(id))
			{
				// Determine locations of clones we should have
				CloneSectionList newCloneSections{ GetCloneSectionList(*m_physicsComponents[id].mainBody.b2BodyPtr) };
				std::queue<CloneSection> locationsNeedingHomes;
				std::queue<unsigned> availableIndices;

				// Make a list of locations that should have a clone but currently do not
				for(unsigned i = 0; i < WORLD_NUM_CLONES; ++i)
				{
					bool needsHome{ true };
					for(auto& cloneBody : m_physicsComponents[id].cloneBodyList)
						if(newCloneSections[i] == cloneBody.section)
						{
							needsHome = false;
							break;
						}
					if(needsHome)
						locationsNeedingHomes.push(newCloneSections[i]);
				}

				// Decide which spots the new clones will over-write
				for(unsigned i = 0; i < WORLD_NUM_CLONES; ++i)
				{
					bool indexAvailable{ true };
					for(auto cloneSection : newCloneSections)
						if(m_physicsComponents[id].cloneBodyList[i].section == cloneSection)
						{
							indexAvailable = false;
							break;
						}
					if(indexAvailable)
						availableIndices.push(i);
				}

				// Over-write clone locations
				while(!locationsNeedingHomes.empty())
				{
					d2Assert(!availableIndices.empty() && "No available index for location needing a home.");
					m_physicsComponents[id].cloneBodyList[availableIndices.front()].section = locationsNeedingHomes.front();
					locationsNeedingHomes.pop();
					availableIndices.pop();
				}

				// De-activate clone bodies that need syncing
				b2Body* mainB2BodyPtr{ m_physicsComponents[id].mainBody.b2BodyPtr };
				float angle{ mainB2BodyPtr->GetAngle() };
				float angularVelocity{ mainB2BodyPtr->GetAngularVelocity() };
				for(unsigned i = 0; i < WORLD_NUM_CLONES; ++i)
				{
					CloneBody& cloneBody{ m_physicsComponents[id].cloneBodyList[i] };
					CloneSyncData& cloneSyncData{ m_cloneSyncDataArrays[id][i] };

					cloneSyncData.positionNeedsSync = NeedsSync(cloneBody.b2BodyPtr->GetPosition(), mainB2BodyPtr->GetPosition() + GetCloneOffset(cloneBody.section));
					cloneSyncData.velocityNeedsSync = NeedsSync(cloneBody.b2BodyPtr->GetLinearVelocity(), mainB2BodyPtr->GetLinearVelocity());
					cloneSyncData.angleNeedsSync = NeedsSync(cloneBody.b2BodyPtr->GetAngle(), angle);
					cloneSyncData.angularVelocityNeedsSync = NeedsSync(cloneBody.b2BodyPtr->GetAngularVelocity(), angularVelocity);

					cloneSyncData.needsReactivation = (cloneSyncData.positionNeedsSync || cloneSyncData.angleNeedsSync);
					if(cloneSyncData.needsReactivation)
						cloneBody.b2BodyPtr->SetEnabled(false);
				}

			}
		EmptyPhysicsStep();

		// Now that the proper clone locations are in place, update states as necessary
		for(EntityID id = 0; id < WORLD_MAX_ENTITIES; ++id)
			if(HasPhysics(id) && IsActive(id))
			{
				b2Body* mainB2BodyPtr{ m_physicsComponents[id].mainBody.b2BodyPtr };
				for(unsigned i = 0; i < WORLD_NUM_CLONES; ++i)
				{
					CloneBody& cloneBody{ m_physicsComponents[id].cloneBodyList[i] };
					CloneSyncData& syncData{ m_cloneSyncDataArrays[id][i] };

					if(syncData.positionNeedsSync || syncData.angleNeedsSync)
					{
						b2Vec2 clonePosition{ mainB2BodyPtr->GetPosition() + GetCloneOffset(cloneBody.section) };
						cloneBody.b2BodyPtr->SetTransform(clonePosition, mainB2BodyPtr->GetAngle());
					}
					if(syncData.velocityNeedsSync)
						cloneBody.b2BodyPtr->SetLinearVelocity(mainB2BodyPtr->GetLinearVelocity());
					if(syncData.angularVelocityNeedsSync)
						cloneBody.b2BodyPtr->SetAngularVelocity(mainB2BodyPtr->GetAngularVelocity());
					if(syncData.needsReactivation)
						cloneBody.b2BodyPtr->SetEnabled(true);
				}
			}
		EmptyPhysicsStep();
	}
	bool World::NeedsSync(float actualValue, float perfectValue) const
	{
		float errorMagnitude{ fabs(actualValue - perfectValue) };
		return (errorMagnitude > WORLD_CLONE_SYNC_TOLERANCE_FLT_EPSILONS);
	}
	bool World::NeedsSync(const b2Vec2& actualValue, const b2Vec2& perfectValue) const
	{
		return NeedsSync(actualValue.x, perfectValue.x) ||
			NeedsSync(actualValue.y, perfectValue.y);
	}
	void World::ProcessDestroyBuffer()
	{
		for(EntityID id : m_destroyBuffer)
		{
			// Send notifications to Game
			if(HasFlag(id, FLAG_EXITED))
				if(m_exitListenerPtr)
					m_exitListenerPtr->EntityExited(id);
			if(m_destructionListenerPtr)
				m_destructionListenerPtr->EntityWillBeDestroyed(id);

			// Particle explosion
			if(!HasFlag(id, FLAG_EXITED))
			{
				ComponentBitset requiredComponents = ComponentBitset{}.set(COMPONENT_PARTICLE_EXPLOSION).set(COMPONENT_PHYSICS);
				if(HasComponentSet(id, requiredComponents) && IsActive(id))
					CreateExplosionFromEntity(id, m_particleExplosionComponents[id]);
			}

			RemoveAllFlags(id);
			RemoveAllComponents(id);
		}
		m_destroyBuffer.clear();
	}
	void World::WrapEntities()
	{
		bool doManualWrapping{ false };
		for(EntityID id = 0; id < WORLD_MAX_ENTITIES; ++id)
		{
			if(HasPhysics(id) && IsActive(id))
			{
				const b2Vec2& currentPosition{ m_physicsComponents[id].mainBody.b2BodyPtr->GetPosition() };
				m_physicsWrapDatas[id].requiresManualWrapping = false;
				m_physicsWrapDatas[id].crossedLeftBound = (currentPosition.x < m_worldRect.lowerBound.x);
				m_physicsWrapDatas[id].crossedRightBound = (currentPosition.x > m_worldRect.upperBound.x);
				m_physicsWrapDatas[id].crossedLowerBound = (currentPosition.y < m_worldRect.lowerBound.y);
				m_physicsWrapDatas[id].crossedUpperBound = (currentPosition.y > m_worldRect.upperBound.y);

				// Figure out if we can simply switch Box2D bodies or if we have to wrap manually
				// (we should be able to switch unless an object is going super fast such that
				// the corresponding clone section is not found among the existing clones)
				CloneSection newCloneSectionAfterSwitch;
				bool wrapNeeded{ GetCloneSectionFromWrapData(m_physicsWrapDatas[id], newCloneSectionAfterSwitch) };
				if(!wrapNeeded)
					continue;
				CloneSection cloneSectionWithWhichToSwitchB2BodyPointers{ GetOpposingCloneSection(newCloneSectionAfterSwitch) };

				// Find clone to switch b2Body pointers
				unsigned cloneIndex;
				bool cloneIndexFound{ false };
				for(const CloneBody& cloneBody : m_physicsComponents[id].cloneBodyList)
				{
					if(cloneBody.section == cloneSectionWithWhichToSwitchB2BodyPointers)
					{
						cloneIndex = cloneBody.cloneIndex;
						cloneIndexFound = true;
						break;
					}
				}

				// Switch if proper clone found
				if(cloneIndexFound)
				{
					// If we're doing the switch method, make sure we don't do the backup method
					m_physicsWrapDatas[id].requiresManualWrapping = false;

					// Wrap saved states
					b2Vec2 translation{ m_physicsComponents[id].cloneBodyList[cloneIndex].b2BodyPtr->GetPosition() - currentPosition };
					m_lastTransforms[id].p += translation;
					m_smoothedTransforms[id].p += translation;

					// Switch b2Body pointers and change section of clone body
					SwitchB2Bodies(m_physicsComponents[id].mainBody, m_physicsComponents[id].cloneBodyList[cloneIndex]);
					m_physicsComponents[id].cloneBodyList[cloneIndex].section = newCloneSectionAfterSwitch;

					// Notify wrap listener
					if(m_wrappedEntityListenerPtr)
						m_wrappedEntityListenerPtr->EntityWrapped(id, translation);

				}
				else
				{
					// Proper clone not found, so flag for manual wrap (should only happen with super fast bodies, if at all)
					// It's possible that this backup method is unnecessary, and could maybe be removed.
					m_physicsWrapDatas[id].requiresManualWrapping = true;
					if(!doManualWrapping)
						doManualWrapping = true;
				}
			}
		}
		SyncClones();

		if(doManualWrapping)
		{
			// De-activate bodies flagged for manual wrap
			for(EntityID id = 0; id < WORLD_MAX_ENTITIES; ++id)
				if(HasPhysics(id) && IsActive(id))
					if(m_physicsWrapDatas[id].requiresManualWrapping)
						m_physicsComponents[id].mainBody.b2BodyPtr->SetEnabled(false);
			EmptyPhysicsStep();

			// Manual wraps
			for(EntityID id = 0; id < WORLD_MAX_ENTITIES; ++id)
				if(HasPhysics(id) && IsActive(id))
					if(m_physicsWrapDatas[id].requiresManualWrapping)
					{
						const b2Transform& currentTransform{ m_physicsComponents[id].mainBody.b2BodyPtr->GetTransform() };
						b2Vec2 translation;
						if(m_physicsWrapDatas[id].crossedLeftBound)
							translation.x = m_worldDimensions.x;
						else if(m_physicsWrapDatas[id].crossedRightBound)
							translation.x = -m_worldDimensions.x;
						else
							translation.x = 0.0f;

						if(m_physicsWrapDatas[id].crossedLowerBound)
							translation.y = m_worldDimensions.y;
						else if(m_physicsWrapDatas[id].crossedRightBound)
							translation.y = -m_worldDimensions.y;
						else
							translation.y = 0.0f;

						if(translation != b2Vec2_zero)
						{
							// Wrap entity
							b2Vec2 wrappedPosition{ m_physicsComponents[id].mainBody.b2BodyPtr->GetPosition() + translation };
							m_physicsComponents[id].mainBody.b2BodyPtr->SetTransform(wrappedPosition, currentTransform.q.GetAngle());

							// Re-activate
							m_physicsComponents[id].mainBody.b2BodyPtr->SetEnabled(true);
							m_physicsWrapDatas[id].requiresManualWrapping = false;

							// Wrap saved states
							m_lastTransforms[id].p += translation;
							m_smoothedTransforms[id].p += translation;

							// Notify wrap listener
							if(m_wrappedEntityListenerPtr)
								m_wrappedEntityListenerPtr->EntityWrapped(id, translation);
						}
					}
		}
		SyncClones();

		for(EntityID id = 0; id < WORLD_MAX_ENTITIES; ++id)
			MoveRadarToMainBody(id);
	}
	//+---------------------------------\-------------------------------------
	//|  GetCloneSectionFromWrapData	| (private)
	//\---------------------------------/
	//	Called by WrapEntities().
	//	If wrap data shows at least one boundary crossed, sets cloneSectionOut
	//		to the clone section that the entity wandered into and returns true.
	//	Otherwise, return false to indicate no wrap is needed.
	//+-----------------------------------------------------------------------
	bool World::GetCloneSectionFromWrapData(const PhysicsWrapData& wrapData, CloneSection& cloneSectionOut) const
	{
		if(wrapData.crossedLeftBound)
		{
			if(wrapData.crossedLowerBound)
				cloneSectionOut = CloneSection::BOTTOM_LEFT;
			else if(wrapData.crossedUpperBound)
				cloneSectionOut = CloneSection::TOP_LEFT;
			else
				cloneSectionOut = CloneSection::LEFT;
		}
		else if(wrapData.crossedRightBound)
		{
			if(wrapData.crossedLowerBound)
				cloneSectionOut = CloneSection::BOTTOM_RIGHT;
			else if(wrapData.crossedUpperBound)
				cloneSectionOut = CloneSection::TOP_RIGHT;
			else
				cloneSectionOut = CloneSection::RIGHT;
		}
		else if(wrapData.crossedLowerBound)
			cloneSectionOut = CloneSection::BOTTOM;
		else if(wrapData.crossedUpperBound)
			cloneSectionOut = CloneSection::TOP;
		else
			return false;
		return true;
	}
	//+---------------------------------\-------------------------------------
	//|     GetOpposingCloneSection     | (private)
	//\---------------------------------/
	//	Called by WrapEntities()
	//+-----------------------------------------------------------------------
	CloneSection World::GetOpposingCloneSection(CloneSection cloneSection) const
	{
		switch(cloneSection)
		{
		case CloneSection::BOTTOM_LEFT:  return CloneSection::TOP_RIGHT;
		case CloneSection::BOTTOM:		 return CloneSection::TOP;
		case CloneSection::BOTTOM_RIGHT: return CloneSection::TOP_LEFT;
		case CloneSection::RIGHT:		 return CloneSection::LEFT;
		case CloneSection::TOP_RIGHT:	 return CloneSection::BOTTOM_LEFT;
		case CloneSection::TOP:			 return CloneSection::BOTTOM;
		case CloneSection::TOP_LEFT:	 return CloneSection::BOTTOM_RIGHT;
		case CloneSection::LEFT: default: return CloneSection::RIGHT;
		}
	}
	void World::SwitchB2Bodies(Body& body1, Body& body2)
	{
		b2Body* tempPtr = body1.b2BodyPtr;
		SetB2BodyPtr(&body1, body2.b2BodyPtr);
		SetB2BodyPtr(&body2, tempPtr);
	}
	void World::SmoothStates(float timestepAlpha)
	{
		// Use current transform for static bodies, otherwise use interpolated transform
		for(EntityID id = 0; id < WORLD_MAX_ENTITIES; ++id)
			if(HasPhysics(id) && IsActive(id))
			{
				if(m_physicsComponents[id].mainBody.b2BodyPtr->GetType() == b2_staticBody)
					m_smoothedTransforms[id] = m_physicsComponents[id].mainBody.b2BodyPtr->GetTransform();
				else
					m_smoothedTransforms[id] = d2d::Lerp(m_lastTransforms[id], m_physicsComponents[id].mainBody.b2BodyPtr->GetTransform(), timestepAlpha);
			}

		m_particleSystem.SmoothStates(timestepAlpha);
	}
	//+------------------------\----------------------------------
	//|	  Physics Callbacks	   |
	//\------------------------/----------------------------------
	bool World::ShouldCollide(b2Fixture* fixturePtr1, b2Fixture* fixturePtr2)
	{
		Body* bodyPtr1 = GetUserBodyFromFixture(fixturePtr1);
		Body* bodyPtr2 = GetUserBodyFromFixture(fixturePtr2);
		d2Assert(bodyPtr1 && bodyPtr2);
		d2Assert(bodyPtr1->b2BodyPtr && bodyPtr2->b2BodyPtr);

		// Ignore disabled collisions
		if(m_physicsComponents[bodyPtr1->entityID].disableCollisions ||
			m_physicsComponents[bodyPtr2->entityID].disableCollisions)
			return false;

		// Ignore clone vs. clone collisions
		if(WORLD_IGNORE_CLONE_VS_CLONE_COLLISIONS)
			if(bodyPtr1->isClone && bodyPtr2->isClone)
				return false;

		// Collisions with exit
		EntityID id1{ bodyPtr1->entityID };
		EntityID id2{ bodyPtr2->entityID };
		PreSolveExit(fixturePtr1, fixturePtr2);
		PreSolveExit(fixturePtr2, fixturePtr1);

		// Ignore collisions with parents (if flag is set)
		bool entity1IsParentOf2{ HasComponent(id2, COMPONENT_PARENT) && m_parentComponents[id2] == id1 };
		bool entity2IsParentOf1{ HasComponent(id1, COMPONENT_PARENT) && m_parentComponents[id1] == id2 };
		if((entity2IsParentOf1 && HasFlag(id1, FLAG_IGNORE_PARENT_COLLISIONS_UNTIL_FIRST_CONTACT_END)) ||
			(entity1IsParentOf2 && HasFlag(id2, FLAG_IGNORE_PARENT_COLLISIONS_UNTIL_FIRST_CONTACT_END)))
			return false;
		else
			return ShouldCollideDefaultFiltering(fixturePtr1->GetFilterData(), fixturePtr2->GetFilterData());
	}
	bool World::ShouldCollideDefaultFiltering(const b2Filter& filter1, const b2Filter& filter2) const
	{
		if(filter1.groupIndex == filter2.groupIndex && filter1.groupIndex != 0)
			return filter1.groupIndex > 0;
		else
			return (filter1.maskBits & filter2.categoryBits) != 0 &&
			(filter1.categoryBits & filter2.maskBits) != 0;
	}
	void World::BeginContact(b2Contact* contactPtr)
	{
		d2Assert(contactPtr && "Box2D Bug");
		BeginContactRadar(contactPtr->GetFixtureA(), contactPtr->GetFixtureB());
		BeginContactRadar(contactPtr->GetFixtureB(), contactPtr->GetFixtureA());
	}
	void World::BeginContactRadar(b2Fixture* fixturePtr1, b2Fixture* fixturePtr2)
	{
		if(fixturePtr1 && fixturePtr2)
		{
			if(fixturePtr1->GetUserData().isRadar && !fixturePtr2->IsSensor())
			{
				std::cout << "BeginContactRadar hit" << std::endl;
				Body* bodyPtr1 = GetUserBodyFromFixture(fixturePtr1);
				Body* bodyPtr2 = GetUserBodyFromFixture(fixturePtr2);
				if(bodyPtr1 && bodyPtr2)
				{
					EntityID id1 = bodyPtr1->entityID;
					if(HasComponent(id1, COMPONENT_RADAR))
					{
						if(m_radarComponents[id1].bodiesInRange.count(bodyPtr2))
							m_radarComponents[id1].bodiesInRange[bodyPtr2]++;
						else
							m_radarComponents[id1].bodiesInRange[bodyPtr2] = 1;				
					}
				}
			}
		}
	}
	void World::PreSolve(b2Contact* contactPtr, const b2Manifold* oldManifoldPtr)
	{
		d2Assert(contactPtr && oldManifoldPtr && "Box2D Bug");
		Body* bodyPtr1 = GetUserBodyFromFixture(contactPtr->GetFixtureA());
		Body* bodyPtr2 = GetUserBodyFromFixture(contactPtr->GetFixtureB());

		if(bodyPtr1 && bodyPtr2)
		{
			EntityID id1 = bodyPtr1->entityID;
			EntityID id2 = bodyPtr2->entityID;

			PreSolveIconCollector(id1, id2, contactPtr);
			PreSolveIconCollector(id2, id1, contactPtr);
		}
	}
	void World::PreSolveExit(b2Fixture* fixturePtr1, b2Fixture* fixturePtr2)
	{
		if(fixturePtr1 && fixturePtr2)
		{
			EntityID id1 = GetUserBodyFromFixture(fixturePtr1)->entityID;
			EntityID id2 = GetUserBodyFromFixture(fixturePtr2)->entityID;
		if(HasFlag(id1, FLAG_EXIT))
				if(!fixturePtr2->IsSensor())
			Exit(id2);
	}
	}
	void World::Exit(EntityID id)
	{
		//// Turn off all components except graphics
		//BitMask componentsToKeep = COMPONENT_NONE;
		//if(HasComponents(id, COMPONENT_DRAW_ANIMATION))
		//	componentsToKeep |= COMPONENT_DRAW_ANIMATION;
		//if(HasComponents(id, COMPONENT_DRAW_FIXTURES))
		//	componentsToKeep |= COMPONENT_DRAW_FIXTURES;
		//RemoveAllComponentsExcept(id, componentsToKeep);

		SetFlag(id, FLAG_EXITED, true);
		Destroy(id);
	}
	void World::PreSolveIconCollector(EntityID id1, EntityID id2, b2Contact* contactPtr)
	{
		bool isCollector1 = HasComponent(id1, COMPONENT_ICON_COLLECTOR);
		bool isIcon2 = HasComponent(id2, COMPONENT_POWERUP) && m_powerUpComponents[id2].type == PowerUpType::ICON;

		if(isCollector1 && isIcon2)
		{
			if(m_iconCollectorComponents[id1].creditsPtr)
				*m_iconCollectorComponents[id1].creditsPtr += (float)m_powerUpComponents[id2].value;
			m_powerUpComponents[id2].value = 0;
			contactPtr->SetEnabled(false);
			Destroy(id2);
		}
	}
	void World::PostSolve(b2Contact* contactPtr, const b2ContactImpulse* impulsePtr)
	{
		d2Assert(contactPtr && impulsePtr && "Box2D Bug");

		// Get user bodies
		Body* bodyPtr1 = GetUserBodyFromFixture(contactPtr->GetFixtureA());
		Body* bodyPtr2 = GetUserBodyFromFixture(contactPtr->GetFixtureB());
		if(!bodyPtr1 || !bodyPtr2)
			return;
		if(bodyPtr1->isClone && bodyPtr2->isClone)
			return;
		if(!bodyPtr1->b2BodyPtr || !bodyPtr2->b2BodyPtr)
			return;

		// Get manifold
		b2Manifold* manifoldPtr{ contactPtr->GetManifold() };
		d2Assert(manifoldPtr && "Box2D Bug");
		int numManifoldPoints{ manifoldPtr->pointCount };
		d2Assert(numManifoldPoints >= 0 && numManifoldPoints <= b2_maxManifoldPoints && "Box2D Bug");

		// Impulse damage
		ApplyImpulseDamage(bodyPtr1, bodyPtr2, impulsePtr->normalImpulses, numManifoldPoints);
	}
	void World::SayGoodbye(b2Joint* joint)
	{

	}
	void World::SayGoodbye(b2Fixture* fixture)
	{

	}
	void World::ApplyImpulseDamage(Body* bodyPtr1, Body* bodyPtr2, const float* normalImpulses, unsigned numImpulses)
	{
		// Get impulse
		std::stringstream damageLog;
		float totalDamage{ 0.0f };
		{
			float impulse{ 0.0f };
			if(m_settings.damageLogging)
				damageLog << "PostSolve: Impulses(" << numImpulses << "): ";

			// Calculate final impulse
			for(unsigned i = 0; i < numImpulses; ++i)
			{
				if(m_settings.addImpulsesForDamages)
					impulse += normalImpulses[i];
				else
					impulse = std::max(impulse, normalImpulses[i]);
				if(m_settings.damageLogging)
					damageLog << normalImpulses[i] << ' ';
			}
			if(m_settings.damageLogging)
			{
				if(m_settings.addImpulsesForDamages)
					damageLog << "Total Impulse: " << impulse;
				else
					damageLog << "Max Impulse: " << impulse;

				damageLog << "     IDs: " << bodyPtr1->entityID;
				if(bodyPtr1->isClone)
					damageLog << "(clone)";
				damageLog << " and " << bodyPtr2->entityID;
				if(bodyPtr2->isClone)
					damageLog << "(clone)";
				damageLog << '\n';
			}

			// Get total damage
			totalDamage = impulse * m_settings.damageToImpulseRatio;
		}

		// If collision big enough to be worth it
		if(totalDamage >= m_settings.minTotalCollisionDamage)
		{
			// Calculate individual damages
			if(m_settings.damageLogging)
				damageLog << "     Total Damage: " << std::setw(8) << totalDamage;
			float damage1, damage2;
			damage1 = damage2 = 0.5f * totalDamage;

			// Apply damage 1
			if(!bodyPtr1->isClone)
			{
				AdjustHealth(bodyPtr1->entityID, -damage1);
				if(m_settings.damageLogging)
					damageLog << " Damage: " << damage1 << " on entity " << bodyPtr1->entityID;
			}

			// Apply damage 2
			if(!bodyPtr2->isClone)
			{
				AdjustHealth(bodyPtr2->entityID, -damage2);
				if(m_settings.damageLogging)
					damageLog << " Damage: " << damage2 << " on entity " << bodyPtr2->entityID;
			}
			if(m_settings.damageLogging) damageLog << '\n';
		}
		if(m_settings.damageLogging)
			d2LogInfo << damageLog.str();
	}
	void World::EndContact(b2Contact* contactPtr)
	{
		// Establish the pair of bodies in contact
		d2Assert(contactPtr && "Box2D Bug");
		std::array<Body*, 2> bodyPtrs{
			GetUserBodyFromFixture(contactPtr->GetFixtureA()),
				GetUserBodyFromFixture(contactPtr->GetFixtureB()) };
		d2Assert(bodyPtrs[0] && bodyPtrs[1]);

		// COMPONENT_DESTRUCTION_DELAY_ON_CONTACT
		for(Body* bodyPtr : bodyPtrs)
			if(!bodyPtr->isClone && HasComponent(bodyPtr->entityID, COMPONENT_DESTRUCTION_DELAY_ON_CONTACT))
			{
				AddDestructionDelayComponent(bodyPtr->entityID, m_destructionDelayOnContactComponents[bodyPtr->entityID]);
				RemoveComponent(bodyPtr->entityID, COMPONENT_DESTRUCTION_DELAY_ON_CONTACT);
			}

		// COMPONENT_DESTRUCTION_CHANCE_ON_CONTACT
		for(Body* bodyPtr : bodyPtrs)
			if(!bodyPtr->isClone && HasComponent(bodyPtr->entityID, COMPONENT_DESTRUCTION_CHANCE_ON_CONTACT))
				if(d2d::RandomFloatPercent() <= m_destructionChanceOnContactComponents[bodyPtr->entityID])
					Destroy(bodyPtr->entityID);

		// FLAG_IGNORE_PARENT_COLLISIONS_UNTIL_FIRST_CONTACT_END
		EntityID id1{ bodyPtrs[0]->entityID };
		EntityID id2{ bodyPtrs[1]->entityID };
		bool entity1IsParentOf2{ HasComponent(id2, COMPONENT_PARENT) && m_parentComponents[id2] == id1 };
		bool entity2IsParentOf1{ HasComponent(id1, COMPONENT_PARENT) && m_parentComponents[id1] == id2 };
		if(entity2IsParentOf1 && HasFlag(id1, FLAG_IGNORE_PARENT_COLLISIONS_UNTIL_FIRST_CONTACT_END))
			SetFlag(id1, FLAG_IGNORE_PARENT_COLLISIONS_UNTIL_FIRST_CONTACT_END, false);
		if(entity1IsParentOf2 && HasFlag(id2, FLAG_IGNORE_PARENT_COLLISIONS_UNTIL_FIRST_CONTACT_END))
			SetFlag(id2, FLAG_IGNORE_PARENT_COLLISIONS_UNTIL_FIRST_CONTACT_END, false);

		// Remove bodies from radar components
		EndContactRadar(contactPtr->GetFixtureA(), contactPtr->GetFixtureB());
		EndContactRadar(contactPtr->GetFixtureB(), contactPtr->GetFixtureA());
	}
	void World::EndContactRadar(b2Fixture* fixturePtr1, b2Fixture* fixturePtr2)
	{
		if(fixturePtr1 && fixturePtr2)
		{
			if(fixturePtr1->GetUserData().isRadar && !fixturePtr2->IsSensor())
			{
				Body* bodyPtr1 = GetUserBodyFromFixture(fixturePtr1);
				Body* bodyPtr2 = GetUserBodyFromFixture(fixturePtr2);
				if(bodyPtr1 && bodyPtr2)
				{
					EntityID id1 = bodyPtr1->entityID;
					if(HasComponent(id1, COMPONENT_RADAR))
					{
						if(m_radarComponents[id1].bodiesInRange.count(bodyPtr2))
						{
							m_radarComponents[id1].bodiesInRange[bodyPtr2]--;
							if(m_radarComponents[id1].bodiesInRange[bodyPtr2] < 1)
								m_radarComponents[id1].bodiesInRange.erase(bodyPtr2);
						}
					}

				}
			}
		}
	}
	void World::AdjustHealth(EntityID entityID, float healthChange)
	{
		if(HasComponent(entityID, COMPONENT_HEALTH))
		{
			if(m_healthComponents[entityID].hp + healthChange <= 0.0f)
				ReduceHealthToZero(entityID);
			else
				m_healthComponents[entityID].hp += healthChange;
		}
	}
	void World::ReduceHealthToZero(EntityID entityID)
	{
		if(HasComponent(entityID, COMPONENT_HEALTH))
		{
			if(m_healthComponents[entityID].hp > 0.0f)
			{
				m_healthComponents[entityID].deathDamage = m_healthComponents[entityID].hp;
				m_healthComponents[entityID].hp = 0.0f;
			}
			Destroy(entityID);
		}
	}
	void World::CreateExplosionFromEntity(EntityID entityID, const ParticleExplosionComponent& particleExplosion)
	{
		unsigned numParticles{ particleExplosion.numParticles };
		int numParticlesOverflowing{ (int)(m_particleSystem.firstUnusedIndex + numParticles) - (int)MAX_PARTICLES };
		if(numParticlesOverflowing > 0)
		{
			d2LogError << "Error: Ran out of particles!";
			numParticles -= numParticlesOverflowing;
		}

		float WEIGHT_OF_CURRENT_VELOCITY{ 0.2f };
		b2Vec2 explosionVelocity{ d2d::Lerp(m_lastLinearVelocities[entityID], m_physicsComponents[entityID].mainBody.b2BodyPtr->GetLinearVelocity(), WEIGHT_OF_CURRENT_VELOCITY) };
		float deathDamage{ HasComponent(entityID, COMPONENT_HEALTH) ? m_healthComponents[entityID].deathDamage : 0.0f };

		// Add particles before modifying values
		ParticleID firstIndex{ m_particleSystem.firstUnusedIndex };
		m_particleSystem.firstUnusedIndex += numParticles;

		// Timers
		for(ParticleID i = firstIndex; i < m_particleSystem.firstUnusedIndex; ++i)
		{
			m_particleSystem.timers[i].age = 0.0f;
			m_particleSystem.timers[i].fadeIn = particleExplosion.fadeIn;
			m_particleSystem.timers[i].fadeOut = particleExplosion.fadeOut;
			m_particleSystem.timers[i].lifetime = particleExplosion.lifetime;
		}

		// Movements and sizes
		for(ParticleID i = firstIndex; i < m_particleSystem.firstUnusedIndex; ++i)
		{
			// Random direction
			float randomAngle{ d2d::RandomFloat({0.0f, d2d::TWO_PI}) };
			b2Vec2 randomUnitVector{ cosf(randomAngle), sinf(randomAngle) };

			// Random point in that direction
			float diameter{ particleExplosion.relativeSize * (m_sizeComponents[entityID].x + m_sizeComponents[entityID].y) * 0.5f };
			float randomRadius{ d2d::RandomFloat({0.0f, 0.5f * diameter}) };
			m_particleSystem.physics[i].position = randomRadius * randomUnitVector + m_smoothedTransforms[entityID].p;

			// Random relativeSize
			int randomSizeIndex{ d2d::RandomInt(particleExplosion.sizeIndexRange) };
			m_particleSystem.pointSizeIndices[i] = randomSizeIndex;

			// Speed based on relativeSize
			float percentOfWayToMaxSpeed;
			if(particleExplosion.sizeIndexRange.GetSize() == 0)
				percentOfWayToMaxSpeed = 1.0f;
			else
				percentOfWayToMaxSpeed = 1.0f - ((float)(randomSizeIndex - particleExplosion.sizeIndexRange.GetMin()) / (float)(particleExplosion.sizeIndexRange.GetSize()));
			float speed{ (percentOfWayToMaxSpeed * (particleExplosion.speedRange.GetSize())) + particleExplosion.speedRange.GetMin() };

			// Speed increased proportional to damage
			if(particleExplosion.damageBasedSpeedIncreaseFactor > 0.0f)
				speed += d2d::RandomFloat({ 0.0f, particleExplosion.damageBasedSpeedIncreaseFactor * deathDamage });

			// Vary speed and angle randomly and factor in the general velocity of the explosion
			const float maxSpeedFluctuationPercent{ 0.2f };
			const float maxAngleFluctuation{ d2d::PI_OVER_SIX };
			float randomSpeedFluctuationFactor{ d2d::RandomFloat({1.0f - maxSpeedFluctuationPercent, 1.0f + maxSpeedFluctuationPercent}) };
			float randomAngleFluctuation{ d2d::RandomFloat({-maxAngleFluctuation, maxAngleFluctuation}) };
			randomAngle += randomAngleFluctuation;
			randomUnitVector.Set(cosf(randomAngle), sinf(randomAngle));
			m_particleSystem.physics[i].velocity = randomSpeedFluctuationFactor * speed * randomUnitVector + explosionVelocity;
		}

		// Layers
		for(ParticleID i = firstIndex; i < m_particleSystem.firstUnusedIndex; ++i)
		{
			int newLayer{ m_drawAnimationComponents[entityID].layer };
			d2d::RandomBool() ? ++newLayer : --newLayer;
			d2d::Clamp(newLayer, m_settings.drawLayerRange);
			m_particleSystem.layers[i] = newLayer;
		}

		// Colors
		for(ParticleID i = firstIndex; i < m_particleSystem.firstUnusedIndex; ++i)
			m_particleSystem.colors[i] = particleExplosion.colorRange.Lerp(d2d::RandomFloatPercent());
	}
	//+------------------------\----------------------------------
	//|	   Box2D user data     |
	//\------------------------/----------------------------------
	Body* World::GetUserBodyPtr(b2Body* b2BodyPtr) const
	{
		if(b2BodyPtr)
			return reinterpret_cast<Body*>(b2BodyPtr->GetUserData().pointer);
		return nullptr;
	}
	Body* World::GetUserBodyFromFixture(b2Fixture* fixturePtr)
	{
		d2Assert(fixturePtr && "Box2D Bug");

		b2Body* b2BodyPtr{ fixturePtr->GetBody() };
		d2Assert(b2BodyPtr && "Box2D Bug");

		Body* bodyPtr{ GetUserBodyPtr(b2BodyPtr) };
		d2Assert(bodyPtr);
		d2Assert(bodyPtr->b2BodyPtr);
		return bodyPtr;
	}
	void World::SetB2BodyPtr(Body* bodyPtr, b2Body* b2BodyPtr)
	{
		if(bodyPtr)
		{
			bodyPtr->b2BodyPtr = b2BodyPtr;
			if(bodyPtr->b2BodyPtr)
			{
				bodyPtr->b2BodyPtr->GetUserData().pointer = reinterpret_cast<uintptr_t>(bodyPtr);
			}
		}
	}
	void World::SwapB2Bodies(Body& body1, Body& body2)
	{
		// ************************     TODO     ***********************************
	}
	//+------------------------\----------------------------------
	//|	 Clone-aware modifiers |
	//\------------------------/----------------------------------
	void World::SetTransform(EntityID entityID, const b2Vec2& position, float angle)
	{
		if(HasPhysics(entityID))
		{
			m_physicsComponents[entityID].mainBody.b2BodyPtr->SetTransform(position, angle);
			m_lastTransforms[entityID] = m_smoothedTransforms[entityID] = m_physicsComponents[entityID].mainBody.b2BodyPtr->GetTransform();
			for(const CloneBody& cloneBody : m_physicsComponents[entityID].cloneBodyList)
				cloneBody.b2BodyPtr->SetTransform(position + GetCloneOffset(cloneBody.section), angle);
		}
	}
	void World::SetLinearVelocity(EntityID entityID, const b2Vec2& velocity)
	{
		if(HasPhysics(entityID))
		{
			m_physicsComponents[entityID].mainBody.b2BodyPtr->SetLinearVelocity(velocity);
			for(const CloneBody& cloneBody : m_physicsComponents[entityID].cloneBodyList)
				cloneBody.b2BodyPtr->SetLinearVelocity(velocity);
		}
	}
	void World::SetAngularVelocity(EntityID entityID, float angularVelocity)
	{
		if(HasPhysics(entityID))
		{
			m_physicsComponents[entityID].mainBody.b2BodyPtr->SetAngularVelocity(angularVelocity);
			for(const CloneBody& cloneBody : m_physicsComponents[entityID].cloneBodyList)
				cloneBody.b2BodyPtr->SetAngularVelocity(angularVelocity);
		}
	}
	void World::Activate(EntityID entityID)
	{
		SetFlag(entityID, FLAG_ACTIVE, true);
		if(HasPhysics(entityID))
		{
			m_physicsComponents[entityID].mainBody.b2BodyPtr->SetEnabled(true);
			for(const CloneBody& cloneBody : m_physicsComponents[entityID].cloneBodyList)
				cloneBody.b2BodyPtr->SetEnabled(true);
		}
	}
	void World::Deactivate(EntityID entityID)
	{
		SetFlag(entityID, FLAG_ACTIVE, false);
		if(HasPhysics(entityID))
		{
			m_physicsComponents[entityID].mainBody.b2BodyPtr->SetEnabled(false);
			for(const CloneBody& cloneBody : m_physicsComponents[entityID].cloneBodyList)
				cloneBody.b2BodyPtr->SetEnabled(false);
		}
	}
	void World::ApplyForceToCenter(EntityID entityID, const b2Vec2& force)
	{
		if(HasPhysics(entityID))
		{
			m_physicsComponents[entityID].mainBody.b2BodyPtr->ApplyForceToCenter(force, true);
			for(const CloneBody& cloneBody : m_physicsComponents[entityID].cloneBodyList)
				cloneBody.b2BodyPtr->ApplyForceToCenter(force, true);
		}
	}
	void World::ApplyForceToLocalPoint(EntityID entityID, const b2Vec2& force, const b2Vec2& localPoint)
	{
		if(HasPhysics(entityID))
		{
			m_physicsComponents[entityID].mainBody.b2BodyPtr->ApplyForce(force, m_physicsComponents[entityID].mainBody.b2BodyPtr->GetWorldPoint(localPoint), true);
			for(const CloneBody& cloneBody : m_physicsComponents[entityID].cloneBodyList)
				cloneBody.b2BodyPtr->ApplyForce(force, cloneBody.b2BodyPtr->GetWorldPoint(localPoint), true);
		}
	}
	void World::ApplyForceToWorldPoint(EntityID entityID, const b2Vec2& force, const b2Vec2& worldPoint)
	{
		if(HasPhysics(entityID))
		{
			m_physicsComponents[entityID].mainBody.b2BodyPtr->ApplyForce(force, worldPoint, true);
			b2Vec2 localPoint{ m_physicsComponents[entityID].mainBody.b2BodyPtr->GetLocalPoint(worldPoint) };
			for(const CloneBody& cloneBody : m_physicsComponents[entityID].cloneBodyList)
				cloneBody.b2BodyPtr->ApplyForce(force, cloneBody.b2BodyPtr->GetWorldPoint(localPoint), true);
		}
	}
	void World::ApplyTorque(EntityID entityID, float torque)
	{
		if(HasPhysics(entityID))
		{
			m_physicsComponents[entityID].mainBody.b2BodyPtr->ApplyTorque(torque, true);
			for(const CloneBody& cloneBody : m_physicsComponents[entityID].cloneBodyList)
				cloneBody.b2BodyPtr->ApplyTorque(torque, true);
		}
	}
	void World::ApplyLinearImpulseToCenter(EntityID entityID, const b2Vec2& impulse)
	{
		if(HasPhysics(entityID))
		{
			m_physicsComponents[entityID].mainBody.b2BodyPtr->ApplyLinearImpulseToCenter(impulse, true);
			for(const CloneBody& cloneBody : m_physicsComponents[entityID].cloneBodyList)
				cloneBody.b2BodyPtr->ApplyLinearImpulseToCenter(impulse, true);
		}
	}
	void World::ApplyLinearImpulseToLocalPoint(EntityID entityID, const b2Vec2& impulse, const b2Vec2& localPoint)
	{
		if(HasPhysics(entityID))
		{
			m_physicsComponents[entityID].mainBody.b2BodyPtr->ApplyLinearImpulse(impulse, m_physicsComponents[entityID].mainBody.b2BodyPtr->GetWorldPoint(localPoint), true);
			for(const CloneBody& cloneBody : m_physicsComponents[entityID].cloneBodyList)
				cloneBody.b2BodyPtr->ApplyLinearImpulse(impulse, cloneBody.b2BodyPtr->GetWorldPoint(localPoint), true);
		}
	}
	void World::ApplyLinearImpulseToWorldPoint(EntityID entityID, const b2Vec2& impulse, const b2Vec2& worldPoint)
	{
		if(HasPhysics(entityID))
		{
			m_physicsComponents[entityID].mainBody.b2BodyPtr->ApplyLinearImpulse(impulse, worldPoint, true);
			b2Vec2 localPoint{ m_physicsComponents[entityID].mainBody.b2BodyPtr->GetLocalPoint(worldPoint) };
			for(const CloneBody& cloneBody : m_physicsComponents[entityID].cloneBodyList)
				cloneBody.b2BodyPtr->ApplyLinearImpulse(impulse, cloneBody.b2BodyPtr->GetWorldPoint(localPoint), true);
		}
	}
	void World::ApplyAngularImpulse(EntityID entityID, float impulse)
	{
		if(HasPhysics(entityID))
		{
			m_physicsComponents[entityID].mainBody.b2BodyPtr->ApplyAngularImpulse(impulse, true);
			for(const CloneBody& cloneBody : m_physicsComponents[entityID].cloneBodyList)
				cloneBody.b2BodyPtr->ApplyAngularImpulse(impulse, true);
		}
	}
	//+----------------------\------------------------------------
	//|		   Joints		 |
	//\----------------------/------------------------------------
	//b2PrismaticJoint* World::ApplyPrismaticJoint(EntityID id1, EntityID id2, const b2Vec2& localAxis1,
	//	const b2Vec2& localAnchor1, const b2Vec2& localAnchor2, float referenceAngle,
	//	bool enableLimit, const d2d::Range<float>& translationRange)
	//{
	//	if(HasPhysics(id1) && HasPhysics(id2))
	//	{
	//		b2PrismaticJointDef prismaticJointDef;
	//		prismaticJointDef.bodyA = m_physicsComponents[id1].mainBody.b2BodyPtr;
	//		prismaticJointDef.bodyB = m_physicsComponents[id2].mainBody.b2BodyPtr;
	//		prismaticJointDef.collideConnected = false;

	//		//local axis, relative to bodyA
	//		prismaticJointDef.localAxisA = localAxis1;

	//		//anchor points
	//		prismaticJointDef.localAnchorA = localAnchor1;
	//		prismaticJointDef.localAnchorB = localAnchor2;

	//		prismaticJointDef.referenceAngle = referenceAngle;

	//		//limits
	//		prismaticJointDef.enableLimit = enableLimit;
	//		prismaticJointDef.lowerTranslation = translationRange.GetMin();
	//		prismaticJointDef.upperTranslation = translationRange.GetMax();

	//		//motor
	//		prismaticJointDef.enableMotor = true;
	//		prismaticJointDef.maxMotorForce = 500;
	//		prismaticJointDef.motorSpeed = 0;

	//		if(m_b2WorldPtr)
	//			return (b2PrismaticJoint*)m_b2WorldPtr->CreateJoint(&prismaticJointDef);
	//	}
	//	return nullptr;
	//}

}
