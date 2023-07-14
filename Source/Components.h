/**************************************************************************************\
** File: ClassTemplate.h
** Project: 
** Author: David Leksen
** Date: 
**
** Header file for the ClassTemplate class
**
\**************************************************************************************/
#pragma once
#include "Model.h"
namespace Space
{
	using EntityID = size_t;

	// Components have associated data
	enum ComponentBit : size_t
	{
		COMPONENT_PHYSICS = 0,
		COMPONENT_DRAW_ON_RADAR,
		COMPONENT_DRAW_ANIMATION,
		COMPONENT_DRAW_FIXTURES,
		COMPONENT_HEALTH,
		COMPONENT_PARENT,
		COMPONENT_PARTICLE_EXPLOSION,
		COMPONENT_DESTRUCTION_DELAY,
		COMPONENT_DESTRUCTION_DELAY_ON_CONTACT,
		COMPONENT_DESTRUCTION_CHANCE_ON_CONTACT,
		COMPONENT_ROTATOR,
		COMPONENT_THRUSTER,
		COMPONENT_SET_THRUST_AFTER_DELAY,
		COMPONENT_BRAKE,
		COMPONENT_PRIMARY_PROJECTILE_LAUNCHER,
		COMPONENT_SECONDARY_PROJECTILE_LAUNCHER,
		COMPONENT_FUEL,
		COMPONENT_BOOSTER,
		COMPONENT_ICON_COLLECTOR,
		COMPONENT_POWERUP,
		COMPONENT_AI,
		COMPONENT_RADAR,
		COMPONENT_NUM_BITS
	};
	typedef std::bitset<COMPONENT_NUM_BITS> ComponentBitset;

	// Flags have no associated data
	enum FlagBit : size_t
	{
		FLAG_ACTIVE = 0,
		FLAG_PLAYER_CONTROLLED,
		FLAG_IGNORE_PARENT_COLLISIONS_UNTIL_FIRST_CONTACT_END,
		FLAG_DISABLE_COLLISIONS_ON_CONTACT_END,
		FLAG_DESTRUCTION_ON_ANIMATION_COMPLETION,
		FLAG_EXIT,
		FLAG_EXITED,
		FLAG_NUM_BITS
	};
	typedef std::bitset<FLAG_NUM_BITS> FlagBitset;

	//+---------------------------------------\
	//|			Exposed Components			  |
	//\---------------------------------------/
	struct IconCollectorComponent
	{
		float* creditsPtr;
	};
	struct DrawFixturesComponent
	{
		d2d::Color color;
		bool fill{ false };
	};
	struct DrawRadarComponent
	{
		d2d::Color color;
		unsigned pointSizeIndex{ 1 };
	};
	struct DrawAnimationComponent
	{
		int layer;
		d2d::Animation animation;
	};
	struct ProjectileDef
	{
		Model model;
		d2d::Material material;
		b2Vec2 dimensions;
		bool fixedRotation;
		bool continuousCollisionDetection;
		d2d::Filter filter;
		bool destructionDelay;
		float destructionDelayTime;
		bool destructionDelayOnContact;
		float destructionDelayOnContactTime;
		bool destructionChanceOnContact;
		float destructionChance;
		bool ignoreParentCollisionsUntilFirstContactEnd;
		float acceleration;
		float accelerationTime;
		//float explosionMaxImpulse;
		//float explosionImpulseRadius;
		//const AnimationDef* explosionAnimationDefPtr{ nullptr };
	};
	struct ParticleExplosionComponent
	{
		float relativeSize;
		unsigned numParticles;
		d2d::Range<float> speedRange;
		float damageBasedSpeedIncreaseFactor;
		d2d::Range<int> sizeIndexRange;
		d2d::ColorRange colorRange;
		float lifetime, fadeIn, fadeOut;
	};
	enum class PowerUpType
	{
		FUEL, ICON
	};
	struct PowerUpComponent
	{
		PowerUpType type;
		int value;
	};

	//+---------------------------------------\
	//|			Internal Components			  |
	//\---------------------------------------/
	const unsigned WORLD_NUM_CLONES = 3;
	const unsigned WORLD_MAX_PROJECTILE_LAUNCHER_SLOTS = 5;
	const unsigned WORLD_MAX_THRUSTER_SLOTS = 5;

	enum class Quadrant { TOP_LEFT, TOP_RIGHT, BOTTOM_RIGHT, BOTTOM_LEFT };
	enum class CloneSection { TOP_LEFT, TOP, TOP_RIGHT, RIGHT, BOTTOM_RIGHT, BOTTOM, BOTTOM_LEFT, LEFT };
	typedef std::array<CloneSection, WORLD_NUM_CLONES> CloneSectionList;
	struct CloneSyncData
	{
		bool positionNeedsSync;
		bool velocityNeedsSync;
		bool angleNeedsSync;
		bool angularVelocityNeedsSync;
		bool needsReactivation;
	};
	struct Body
	{
		EntityID entityID;
		bool isClone;
		b2Body* b2BodyPtr;
	};
	struct CloneBody : public Body
	{
		unsigned cloneIndex;
		CloneSection section;
	};
	struct PhysicsComponent
	{
		Body mainBody;
		std::array<CloneBody, WORLD_NUM_CLONES> cloneBodyList;
		bool disableCollisions;
	};
	struct PhysicsWrapData
	{
		bool crossedLeftBound;
		bool crossedRightBound;
		bool crossedLowerBound;
		bool crossedUpperBound;
		bool requiresManualWrapping;
	};
	struct RotatorComponent
	{
		float factor;
		float lastFactor;
		float rotationSpeed;
	};
	struct Thruster
	{
		bool enabled;
		d2d::Animation animation;
		float acceleration;
		b2Vec2 localRelativePosition;
		float fuelPerSecond;
	};
	struct BoosterComponent
	{
		float factor;
		float boostSeconds;
		float cooldownSeconds;

		bool engaged;
		float secondsLeft;
		float cooldownSecondsLeft;
	};
	struct ThrusterComponent
	{
		float factor;
		unsigned numSlots;
		Thruster thrusters[WORLD_MAX_THRUSTER_SLOTS];
	};
	struct SetThrustFactorAfterDelayComponent
	{
		float factor;
		float delay;
	};
	struct FuelComponent
	{
		float level;
		float max;
	};
	struct BrakeComponent
	{
		float factor;
		float deceleration;
	};
	struct HealthComponent
	{
		float hpMax;
		float hp;
		float deathDamage{ 0.0f };
	};
	struct ProjectileLauncher
	{
		bool enabled;
		bool temporarilyDisabled;
		ProjectileDef projectileDef;
		b2Vec2 localRelativePosition;
		float impulse;
		float interval;
		float intervalAccumulator;
	};
	struct ProjectileLauncherComponent
	{
		float factor;
		unsigned numSlots;
		std::array<ProjectileLauncher, WORLD_MAX_PROJECTILE_LAUNCHER_SLOTS> projectileLaunchers;
	};
	enum class AIType
	{
		AI_ROAM
	};
	struct AIComponent
	{
		AIType type;
	};
	struct RadarComponent
	{
		float range;
		std::vector<EntityID> entitiesInRange;
	};
}
