/**************************************************************************************\
** File: World.h
** Project: 
** Author: David Leksen
** Date: 
**
** Header file for the World class
**
\**************************************************************************************/
#pragma once
#include "ParticleSystem.h"
#include "Exceptions.h"
#include "WorldDef.h"

namespace Space
{
	using WorldID = size_t;
	const WorldID WORLD_MAX_ENTITIES{ 10000 };
	const unsigned WORLD_MAX_PROJECTILE_LAUNCHER_SLOTS{ 5 };
	const unsigned WORLD_MAX_THRUSTER_SLOTS{ 5 };
	const unsigned WORLD_NUM_CLONES{ 3 };
	const float WORLD_CLONE_SYNC_TOLERANCE_FLT_EPSILONS{ 4.0f * FLT_EPSILON };
	const d2d::Color WORLD_DEBUG_DRAW_FIXTURES_COLOR{ d2d::WHITE_OPAQUE };
	const unsigned WORLD_MAX_ANIMATION_FRAMES{ 5 };
	const bool WORLD_IGNORE_CLONE_VS_CLONE_COLLISIONS{ false };

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
	struct Model
	{
		std::string name;
		d2d::AnimationDef animationDef;
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
	struct PlayerController
	{
		float primaryFireFactor{};
		float secondaryFireFactor{};
		float turnFactor{};
		float thrustFactor{};
		float brakeFactor{};
		float zoomOutFactor{};
		//bool doMorphOnce{ false };
		int numMissiles{};
	};
	class DestroyListener
	{	
		public: virtual void SayGoodbye(WorldID entityID) = 0;
	};
	class WrapListener 
	{	
		public:	virtual void EntityWrapped(WorldID entityID, const b2Vec2& translation) = 0;
	};
	class ProjectileLauncherCallback
	{
		public:	virtual WorldID LaunchProjectile(const ProjectileDef& projectileDef, const b2Vec2& position,
			float angle, float impulse, const b2Vec2& parentVelocity, WorldID parentID) = 0;
	};
	//class MorphListener
	//{
	//	public:	virtual void MorphedIntoEntity(WorldID replacedEntityID, WorldID newEntityID) = 0;
	//};
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

	// Components have other associated data while flags do not
	typedef long long BitMask;
	enum ComponentBits : BitMask
	{
		COMPONENT_NONE									= 0,
		COMPONENT_PHYSICS								= 1 << 0,
		COMPONENT_DRAW_ON_RADAR 						= 1 << 1,
		COMPONENT_DRAW_ANIMATION  						= 1 << 2,
		COMPONENT_DRAW_FIXTURES							= 1 << 3,
		COMPONENT_HEALTH								= 1 << 4,
		COMPONENT_PARENT								= 1 << 5,
		COMPONENT_PARTICLE_EXPLOSION					= 1 << 6,
		COMPONENT_DESTRUCTION_DELAY						= 1 << 7,
		COMPONENT_DESTRUCTION_DELAY_ON_CONTACT			= 1 << 8,
		COMPONENT_DESTRUCTION_CHANCE_ON_CONTACT			= 1 << 9,
		COMPONENT_ROTATOR 								= 1 << 10,
		COMPONENT_THRUSTER 								= 1 << 11,
		COMPONENT_SET_THRUST_AFTER_DELAY				= 1 << 12,
		COMPONENT_BRAKE 								= 1 << 13,
		COMPONENT_PRIMARY_PROJECTILE_LAUNCHER			= 1 << 14,
		COMPONENT_SECONDARY_PROJECTILE_LAUNCHER			= 1 << 15,
		//COMPONENT_LEVEL_TAG								= 1 << 16
		//COMPONENT_MORPH_INTO_ENTITY_ID					= 1 << 16
	};
	// Unlike components, flags have no associated data
	enum FlagBits : BitMask
	{
		FLAG_NONE									= 0,
		FLAG_ACTIVE									= 1 << 0,
		FLAG_PLAYER_CONTROLLED						= 1 << 1,
		FLAG_IGNORE_PARENT_COLLISIONS_UNTIL_FIRST_CONTACT_END	= 1 << 2,
		FLAG_DISABLE_COLLISIONS_ON_CONTACT_END		= 1 << 3,
		FLAG_DESTRUCTION_ON_ANIMATION_COMPLETION	= 1 << 4
	};	
	//+---------------------------------------------\
	//|  World: b2World wrapper and entity manager  |
	//\---------------------------------------------/
	class World : public b2ContactListener, public b2ContactFilter
	{
	public:
		//+---------------------------------------\
		//|          Public Functions             |
		//\---------------------------------------/
		~World();
		void Init(const d2d::Rect& rect);
		void SetDestructionListener(DestroyListener* listenerPtr);
		void SetWrapListener(WrapListener* listenerPtr);
		void SetProjectileLauncherCallback(ProjectileLauncherCallback* callbackPtr);
		//void SetMorphListener(MorphListener* listenerPtr);
		void Update(float dt, PlayerController& playerController);
		void Draw();

		// Creating entities
		WorldID NewEntityID(const b2Vec2& size, int drawLayer = 0, bool activate = true);
		void SetFlags(WorldID entityID, FlagBits flagBits, bool enable=true);
		void RemoveComponents(WorldID entityID, ComponentBits componentBitMask);

		// Physics
		bool GetRandomPositionAwayFromExistingEntities(float newBoundingRadius,
			float minDistanceToClosestEntity, unsigned maxAttempts, b2Vec2& relativePositionOut) const;
		void AddPhysicsComponent(WorldID entityID, b2BodyType type,
			const b2Vec2& position, float angle, const b2Vec2& velocity = b2Vec2_zero, float angularVelocity = 0.0f,
			bool fixedRotation = false, bool continuousCollisionDetection = false);
		void AddCircleShape(WorldID entityID, const d2d::Material& material, const d2d::Filter& filter,
			float sizeRelativeToWidth = 1.0f, const b2Vec2& position = b2Vec2_zero, bool isSensor = false);
		void AddRectShape(WorldID entityID, const d2d::Material& material, const d2d::Filter& filter,
			const b2Vec2& relativeSize = { 1.0f, 1.0f }, bool isSensor = false,
			const b2Vec2& position = b2Vec2_zero, float angle = 0.0f);
		void AddShapes(WorldID entityID, const std::string& model,
			const d2d::Material& material, const d2d::Filter& filter, bool isSensor = false,
			const b2Vec2& position = b2Vec2_zero, float angle = 0.0f);

		// Visual
		void AddDrawRadarComponent(WorldID entityID, const DrawRadarComponent& radarComponent);
		void AddDrawFixturesComponent(WorldID entityID, const DrawFixturesComponent& fixturesComponent);
		void AddDrawAnimationComponent(WorldID entityID, const d2d::AnimationDef& animationDef);
		void SetAnimationLayer(WorldID entityID, int layer);

		// Life and Death
		void AddHealthComponent(WorldID entityID, float maxHP);
		void AddParentComponent(WorldID entityID, WorldID parentID);
		void AddParticleExplosionOnDeathComponent(WorldID entityID, float relativeSize,
			unsigned numParticles, const d2d::Range<float>& speedRange, float damageBasedSpeedIncreaseFactor,
			const d2d::Range<int>& sizeIndexRange, const d2d::ColorRange& colorRange,
			float lifetime, float fadeIn, float fadeOut);
		void AddDestructionDelayComponent(WorldID entityID, float delay);
		void AddDestructionDelayOnContactComponent(WorldID entityID, float delay);
		void AddDestructionChanceOnContactComponent(WorldID entityID, float delay);
		//void AddMorphIntoEntityID(WorldID entityID, WorldID newEntityID);

		// Canons
		void AddProjectileLauncherComponent(WorldID entityID, unsigned numSlots, bool secondaryLaunchers);
		void AddProjectileLauncher(WorldID entityID, unsigned slot, const ProjectileDef& projectileDef,
			const b2Vec2& localRelativePosition, float impulse, float interval, bool temporarilyDisabled, bool secondaryLaunchers);
		void RemoveProjectileLauncher(WorldID entityID, unsigned slot, bool secondaryLaunchers);
		bool IsValidProjectileLauncherSlot(WorldID entityID, unsigned slot, bool secondaryLaunchers) const;

		// Getting around
		void AddThrusterComponent(WorldID entityID, unsigned numSlots, float initialFactor = 0.0f);
		void AddThruster(WorldID entityID, unsigned slot, const d2d::AnimationDef& animationDef,
			float acceleration, const b2Vec2& localRelativePosition);
		void RemoveThruster(WorldID entityID, unsigned slot);
		bool IsValidThrusterSlot(WorldID entityID, unsigned slot) const;
		void AddSetThrustFactorAfterDelayComponent(WorldID entityID, float thrustFactor, float delay);
		void AddRotatorComponent(WorldID entityID, float rotationSpeed);
		void AddBrakeComponent(WorldID entityID, float deceleration);

		// Query
		bool IsActive(WorldID entityID) const;
		const d2d::Rect& GetWorldRect() const;
		const b2Vec2& GetWorldCenter() const;
		WorldID GetEntityCount() const;
		bool EntityExists(WorldID entityID) const;
		bool HasComponents(WorldID entityID, BitMask componentMask) const;
		bool HasFlags(WorldID entityID, BitMask flagMask) const;
		bool HasSize(WorldID entityID) const;
		bool HasSize2D(WorldID entityID) const;
		bool HasPhysics(WorldID entityID) const;
		bool GetClosestPhysicalEntity(const b2Vec2& position, float boudingRadius,
			WorldID& entityIDOut, float& boundingRadiiGapOut) const;
		int GetDrawLayer(WorldID entityID) const;

		// Callers of the following must ensure entity has a physics component
		const b2Transform& GetSmoothedTransform(WorldID entityID) const;
		const b2Vec2& GetLinearVelocity(WorldID entityID) const;
		float GetAngularVelocity(WorldID entityID) const;
		const b2Vec2& GetLocalCenterOfMass(WorldID entityID) const;
		const b2Vec2& GetWorldCenter(WorldID entityID) const;

		// Box2D callbacks
		bool ShouldCollide(b2Fixture* fixturePtr1, b2Fixture* fixturePtr2) override;
		void BeginContact(b2Contact* contactPtr) override;
		void PreSolve(b2Contact* contactPtr, const b2Manifold* oldManifoldPtr) override;
		void PostSolve(b2Contact* contactPtr, const b2ContactImpulse* impulsePtr) override;
		void EndContact(b2Contact* contactPtr) override;

		// Health modifiers
		void AdjustHealth(WorldID entityID, float healthChange);
		void ReduceHealthToZero(WorldID entityID);

		// Clone-aware modifiers
		void SetTransform(WorldID entityID, const b2Vec2& position, float angle);
		void SetLinearVelocity(WorldID entityID, const b2Vec2& velocity);
		void SetAngularVelocity(WorldID entityID, float angularVelocity);
		void Activate(WorldID entityID);
		void Deactivate(WorldID entityID);
		void ApplyForceToCenter(WorldID entityID, const b2Vec2& force);
		void ApplyForceToLocalPoint(WorldID entityID, const b2Vec2& force, const b2Vec2& localPoint);
		void ApplyForceToWorldPoint(WorldID entityID, const b2Vec2& force, const b2Vec2& worldPoint);
		void ApplyTorque(WorldID entityID, float torque);
		void ApplyLinearImpulseToCenter(WorldID entityID, const b2Vec2& impulse);
		void ApplyLinearImpulseToLocalPoint(WorldID entityID, const b2Vec2& impulse, const b2Vec2& localPoint);
		void ApplyLinearImpulseToWorldPoint(WorldID entityID, const b2Vec2& impulse, const b2Vec2& worldPoint);
		void ApplyAngularImpulse(WorldID entityID, float impulse);

		//// For external use
		//void SetLevelTag(WorldID entityID, int levelTag);
		//void RemoveLevelTag(WorldID entityID);

	private:
		//+---------------------------------------\
		//|         Private Data Types            |
		//\---------------------------------------/
		enum class Quadrant { TOP_LEFT, TOP_RIGHT, BOTTOM_RIGHT, BOTTOM_LEFT };
		enum class CloneSection { TOP_LEFT, TOP, TOP_RIGHT, RIGHT, BOTTOM_RIGHT, BOTTOM, BOTTOM_LEFT, LEFT };
		typedef std::array<CloneSection, WORLD_NUM_CLONES> CloneSectionList;
		struct Body
		{
			WorldID entityID;
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
			bool temporarilyDisabled;
			d2d::Animation animation;
			float acceleration;
			b2Vec2 localRelativePosition;
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
		struct CloneSyncData
		{
			bool positionNeedsSync;
			bool velocityNeedsSync;
			bool angleNeedsSync;
			bool angularVelocityNeedsSync;
			bool needsReactivation;
		};
		struct DamageData
		{
			float damage;
			WorldID entityID;
			b2Vec2 position;
		};
		template<class T> class ComponentArray 
			: public std::array<T, WORLD_MAX_ENTITIES>{};
		typedef std::array<CloneSyncData, WORLD_NUM_CLONES> CloneSyncDataArray;

		//+---------------------------------------\
		//|          Private Funtions             |
		//\---------------------------------------/
		// Updates
		void SingleUpdateStep(float dt, PlayerController& playerController);
		void UpdateEntityCount();
		void UpdateDestructionDelayComponents(float dt);
		void UpdatePlayerControllerComponents(float dt, PlayerController& playerController);
		void UpdateRotatorComponents();
		void UpdateSetThrustFactorAfterDelayComponents(float dt);
		void UpdateThrusterComponents(float dt);
		void UpdateBrakeComponents();
		void UpdateProjectileLauncherComponents(float dt, bool secondaryLaunchers);
		void UpdateDrawAnimationComponents(float dt);

		// Physics
		void UpdatePhysics(float dt);
		void SaveVelocities();
		void EmptyPhysicsStep();
		void ResetSmoothStates();
		void SyncClones();
		bool NeedsSync(float actualValue, float perfectValue) const;
		bool NeedsSync(const b2Vec2& actualValue, const b2Vec2& perfectValue) const;
		Quadrant GetQuadrant(const b2Vec2& position) const;
		CloneSectionList GetCloneSectionList(const b2Vec2& position) const;
		CloneSectionList GetCloneSectionList(const b2Body& b2Body) const;
		b2Vec2 GetCloneOffset(CloneSection cloneSection) const;
		bool GetCloneSectionFromWrapData(const PhysicsWrapData& wrapData, CloneSection& cloneSectionOut) const;
		CloneSection GetOpposingCloneSection(CloneSection cloneSection) const;
		void WrapEntities();
		void SwitchB2Bodies(Body& body1, Body& body2);
		void SmoothStates(float timestepAlpha);
		void ProcessDestroyBuffer();
		bool ShouldCollideDefaultFiltering(const b2Filter& filter1, const b2Filter& filter2) const;
		void CreateExplosionFromEntity(WorldID entityID, const ParticleExplosionComponent& particleExplosion);
		
		// Box2D user data
		Body* GetUserBodyPtr(b2Body* b2BodyPtr) const;
		void SetB2BodyPtr(Body* bodyPtr, b2Body* b2BodyPtr);
		void SwapB2Bodies(Body& body1, Body& body2);

		// Graphics
		void DrawWorldEdge();
		void DrawLayer(int layer);
		void DrawParticleSystem(int layer);
		void DrawAllThrusterComponents(int layer);
		void DrawThrusterComponent(const ThrusterComponent& thrusterComponent, const b2Vec2& entitySize,
			const b2Vec2& position, float angle);
		void DrawAllAnimationComponents(int layer);
		void DrawAnimation(const d2d::Animation& animation, const b2Vec2& size, const b2Vec2& position, float angle);
		void DrawAllFixturesComponents(int layer);
		void DrawFixtureList(b2Fixture* fixturePtr, const b2Vec2& position, float angle, bool fill);
		void DrawAllHealthMeters();
		void DrawHealthMeter(float hp, float hpMax, const b2Vec2& position);
		void DrawRadar();

		//+---------------------------------------\
		//|			    Private Data	          |
		//\---------------------------------------/
		WorldDef m_settings;
		WorldID m_highestActiveEntityCount{ 0 };
		DestroyListener* m_destructionListenerPtr{ nullptr };
		WrapListener* m_wrappedEntityListenerPtr{ nullptr };
		ProjectileLauncherCallback* m_projectileLauncherCallbackPtr{ nullptr };
		//MorphListener* m_morphListenerPtr{ nullptr };

		float m_timestepAccumulator{ 0.0f };
		b2World* m_b2WorldPtr{ nullptr };
		std::queue< WorldID > m_destroyBuffer;
		std::list< DamageData > m_damageDataList;
		b2Vec2 m_worldDimensions;
		b2Vec2 m_worldCenter;
		d2d::Rect m_worldRect;

		// All entities have these by default:
		ComponentArray< BitMask > m_componentBits;
		ComponentArray< BitMask > m_flagBits;
		ComponentArray< bool > m_activeFlags;
		ComponentArray< b2Vec2 > m_sizeComponents;
		ComponentArray< float > m_boundingRadiusComponents;

		// These must be manually added after calling NewEntityID()
		//ComponentArray< int > m_levelTagComponents;
		ComponentArray< HealthComponent > m_healthComponents;
		ComponentArray< float > m_destructionDelayComponents;
		ComponentArray< float > m_destructionDelayOnContactComponents;
		ComponentArray< float > m_destructionChanceOnContactComponents;
		//ComponentArray< WorldID > m_morphIntoEntityIDs;
		ComponentArray< RotatorComponent > m_rotatorComponents;
		ComponentArray< SetThrustFactorAfterDelayComponent > m_setThrustFactorAfterDelayComponents;
		ComponentArray< ThrusterComponent > m_thrusterComponents;
		ComponentArray< BrakeComponent > m_brakeComponents;
		ComponentArray< ProjectileLauncherComponent > m_primaryProjectileLauncherComponents;
		ComponentArray< ProjectileLauncherComponent > m_secondaryProjectileLauncherComponents;
		ComponentArray< WorldID > m_parentComponents;

		ComponentArray< PhysicsComponent > m_physicsComponents;
		ComponentArray< PhysicsWrapData > m_physicsWrapDatas;
		ComponentArray< b2Transform > m_lastTransforms;
		ComponentArray< b2Transform > m_smoothedTransforms;
		ComponentArray< b2Vec2 > m_lastLinearVelocities;
		ComponentArray< CloneSyncDataArray > m_cloneSyncDataArrays;

		ParticleSystem m_particleSystem;
		ComponentArray< ParticleExplosionComponent > m_particleExplosionComponents;
		ComponentArray< DrawAnimationComponent > m_drawAnimationComponents;
		ComponentArray< DrawFixturesComponent > m_drawFixtureComponents;
		ComponentArray< DrawRadarComponent > m_drawRadarComponents;
		
		d2d::ShapeFactory m_shapeFactory;
	};
}
