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
#include "Components.h"
#include "ParticleSystem.h"
#include "WorldDef.h"
#include "WorldUtility.h"
namespace Space
{
	const EntityID WORLD_MAX_ENTITIES = 10000;
	const float WORLD_CLONE_SYNC_TOLERANCE_FLT_EPSILONS = 4.0f * FLT_EPSILON;
	const d2d::Color WORLD_DEBUG_DRAW_FIXTURES_COLOR{ d2d::WHITE_OPAQUE };
	const unsigned WORLD_MAX_ANIMATION_FRAMES = 5;
	const bool WORLD_IGNORE_CLONE_VS_CLONE_COLLISIONS = false;
	const float WORLD_BOOST_FUEL_USE_PENALTY_FACTOR = 2.0f;

	struct InstanceDef
	{
		b2Vec2 position{ b2Vec2_zero };
		float angle{ 0.0f };
		b2Vec2 velocity{ b2Vec2_zero };
		float angularVelocity{ 0.0f };
		bool activate{ true };
	};
	struct PlayerController
	{
		float primaryFireFactor{};
		float secondaryFireFactor{};
		float turnFactor{};
		float thrustFactor{};
		float brakeFactor{};
		bool boost{ false };
		float zoomOutFactor{};
		int numMissiles{};
	};
	class DestroyListener
	{
	public: virtual void EntityWillBeDestroyed(EntityID entityID) = 0;
	};
	class WrapListener
	{
	public:	virtual void EntityWrapped(EntityID entityID, const b2Vec2& translation) = 0;
	};
	class ProjectileLauncherListener
	{
	public:	virtual void ProjectileLaunched(const ProjectileDef& projectileDef, EntityID parentID) = 0;
	};
	class ExitListener
	{
	public:	virtual void EntityExited(EntityID entityID) = 0;
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
		void SetProjectileLauncherListener(ProjectileLauncherListener* listenerPtr);
		void SetExitListener(ExitListener* listenerPtr);
		void Update(float dt, PlayerController& playerController);
		void Draw() const;

		// Creating entities
		EntityID NewEntityID(const b2Vec2& size, int drawLayer = 0, bool activate = true);
		void Destroy(EntityID id);
		void SetFlag(EntityID entityID, FlagBit flagBit, bool enable);
		void SetFlagSet(EntityID entityID, FlagBitset flagBits, bool enable);
		void RemoveAllFlags(EntityID entityID);
		void RemoveComponent(EntityID entityID, ComponentBit componentBit);
		void RemoveComponentSet(EntityID entityID, ComponentBitset componentBits);
		void RemoveAllComponents(EntityID entityID);
		void DestroyB2Bodies(EntityID entityID);
		//void RemoveAllComponentsExcept(EntityID entityID, BitMask componentBitMask);

		// Physics
		bool GetRandomPositionAwayFromExistingEntities(float newBoundingRadius,
			float minDistanceToClosestEntity, unsigned maxAttempts, b2Vec2& relativePositionOut) const;
		void AddPhysicsComponent(EntityID entityID, b2BodyType type,
			const InstanceDef& def, bool fixedRotation = false, bool continuousCollisionDetection = false);
		void AddCircleShape(EntityID entityID, const d2d::Material& material, const d2d::Filter& filter,
			float sizeRelativeToWidth = 1.0f, const b2Vec2& position = b2Vec2_zero, bool isSensor = false);
		void AddRectShape(EntityID entityID, const d2d::Material& material, const d2d::Filter& filter,
			const b2Vec2& relativeSize = { 1.0f, 1.0f }, bool isSensor = false,
			const b2Vec2& position = b2Vec2_zero, float angle = 0.0f);
		void AddShapes(EntityID entityID, const std::string& model,
			const d2d::Material& material, const d2d::Filter& filter, bool isSensor = false,
			const b2Vec2& position = b2Vec2_zero, float angle = 0.0f);

		// Visual
		void AddDrawRadarComponent(EntityID entityID, const DrawRadarComponent& radarComponent);
		void AddDrawFixturesComponent(EntityID entityID, const DrawFixturesComponent& fixturesComponent);
		void AddDrawAnimationComponent(EntityID entityID, const d2d::AnimationDef& animationDef);
		void SetAnimationLayer(EntityID entityID, int layer);

		// Power-ups
		void AddPowerUpComponent(EntityID entityID, const PowerUpComponent& powerUp = {});
		void AddIconCollectorComponent(EntityID entityID, float* creditsPtr);

		// Life and Death
		void AddHealthComponent(EntityID entityID, float maxHP);
		void AddParentComponent(EntityID entityID, EntityID parentID);
		void AddParticleExplosionOnDeathComponent(EntityID entityID, float relativeSize,
			unsigned numParticles, const d2d::Range<float>& speedRange, float damageBasedSpeedIncreaseFactor,
			const d2d::Range<int>& sizeIndexRange, const d2d::ColorRange& colorRange,
			float lifetime, float fadeIn, float fadeOut);
		void AddDestructionDelayComponent(EntityID entityID, float delay);
		void AddDestructionDelayOnContactComponent(EntityID entityID, float delay);
		void AddDestructionChanceOnContactComponent(EntityID entityID, float delay);

		// Canons
		void AddProjectileLauncherComponent(EntityID entityID, unsigned numSlots, bool secondaryLaunchers);
		void AddProjectileLauncher(EntityID entityID, unsigned slot, const ProjectileDef& projectileDef,
			const b2Vec2& localRelativePosition, float impulse, float interval, bool temporarilyDisabled, bool secondaryLaunchers);
		void RemoveProjectileLauncher(EntityID entityID, unsigned slot, bool secondaryLaunchers);
		bool IsValidProjectileLauncherSlot(EntityID entityID, unsigned slot, bool secondaryLaunchers) const;
		EntityID LaunchProjectile(const ProjectileDef& projectileDef, const b2Vec2& position,
			float angle, float impulse, const b2Vec2& parentVelocity, EntityID parentID);

		// Getting around
		void AddThrusterComponent(EntityID entityID, unsigned numSlots, float initialFactor = 0.0f);
		void AddThruster(EntityID entityID, unsigned slot, const d2d::AnimationDef& animationDef,
			float acceleration, float fuelPerSecond, const b2Vec2& localRelativePosition);
		void RemoveThruster(EntityID entityID, unsigned slot);
		bool IsValidThrusterSlot(EntityID entityID, unsigned slot) const;
		void AddSetThrustFactorAfterDelayComponent(EntityID entityID, float thrustFactor, float delay);
		void AddBoosterComponent(EntityID entityID, float factor, float boostSeconds, float cooldownSeconds);
		void AddFuelComponent(EntityID entityID, float level, float max);
		void AddRotatorComponent(EntityID entityID, float rotationSpeed);
		void AddBrakeComponent(EntityID entityID, float deceleration);

		// AI
		void AddAIComponent(EntityID entityID, AIType type);
		void AddRadarComponent(EntityID entityID, float range);

		// Query
		bool IsActive(EntityID entityID) const;
		const d2d::Rect& GetWorldRect() const;
		const b2Vec2& GetWorldCenter() const;
		EntityID GetEntityCount() const;

		bool EntityExists(EntityID entityID) const;
		bool HasComponent(EntityID entityID, ComponentBit componentBit) const;
		bool HasComponentSet(EntityID entityID, ComponentBitset componentBits) const;
		bool HasFlag(EntityID entityID, FlagBit flagBit) const;
		bool HasFlagSet(EntityID entityID, FlagBitset flagBits) const;

		bool HasSize(EntityID entityID) const;
		bool HasSize2D(EntityID entityID) const;
		bool HasPhysics(EntityID entityID) const;

		// Get <id, boundingCircleGap> of count closest entities
		std::list<std::pair<EntityID, float>> GetClosestEntities(const b2Vec2& position, 
			float radius, unsigned count) const;

		// Get id of entities whose bounding circle intersects with given area
		std::vector<EntityID> GetEntitiesInArea(const b2Vec2& position, float radius) const;

		int GetDrawLayer(EntityID entityID) const;
		float GetFuelLevel(EntityID entityID) const;
		float GetMaxFuelLevel(EntityID entityID) const;
		float GetTotalThrusterAcceleration(EntityID id) const;
		float GetTotalThrusterFuelRequired(EntityID id, float dt) const;

		// Callers of the following must ensure entity has a physics component
		const b2Transform& GetSmoothedTransform(EntityID entityID) const;
		const b2Vec2& GetLinearVelocity(EntityID entityID) const;
		float GetAngularVelocity(EntityID entityID) const;
		const b2Vec2& GetLocalCenterOfMass(EntityID entityID) const;
		const b2Vec2& GetWorldCenter(EntityID entityID) const;

		// Box2D callbacks
		bool ShouldCollide(b2Fixture* fixturePtr1, b2Fixture* fixturePtr2) override;
		void BeginContact(b2Contact* contactPtr) override;
		void PreSolve(b2Contact* contactPtr, const b2Manifold* oldManifoldPtr) override;
		void PreSolveExit(EntityID id1, EntityID id2);
		void Exit(EntityID id);
		void PreSolveIconCollector(EntityID id1, EntityID id2, b2Contact* contactPtr);
		void PostSolve(b2Contact* contactPtr, const b2ContactImpulse* impulsePtr) override;
		void EndContact(b2Contact* contactPtr) override;

		// Health modifiers
		void AdjustHealth(EntityID entityID, float healthChange);
		void ReduceHealthToZero(EntityID entityID);

		// Clone-aware modifiers
		void SetTransform(EntityID entityID, const b2Vec2& position, float angle);
		void SetLinearVelocity(EntityID entityID, const b2Vec2& velocity);
		void SetAngularVelocity(EntityID entityID, float angularVelocity);
		void Activate(EntityID entityID);
		void Deactivate(EntityID entityID);
		void ApplyForceToCenter(EntityID entityID, const b2Vec2& force);
		void ApplyForceToLocalPoint(EntityID entityID, const b2Vec2& force, const b2Vec2& localPoint);
		void ApplyForceToWorldPoint(EntityID entityID, const b2Vec2& force, const b2Vec2& worldPoint);
		void ApplyTorque(EntityID entityID, float torque);
		void ApplyLinearImpulseToCenter(EntityID entityID, const b2Vec2& impulse);
		void ApplyLinearImpulseToLocalPoint(EntityID entityID, const b2Vec2& impulse, const b2Vec2& localPoint);
		void ApplyLinearImpulseToWorldPoint(EntityID entityID, const b2Vec2& impulse, const b2Vec2& worldPoint);
		void ApplyAngularImpulse(EntityID entityID, float impulse);
		//b2PrismaticJoint* ApplyPrismaticJoint(EntityID id1, EntityID id2, const b2Vec2& localAxis1,
		//	const b2Vec2& localAnchor1, const b2Vec2& localAnchor2, float referenceAngle = 0.0f,
		//	bool enableLimit = false, const d2d::Range<float>& translationRange = { 0.0f, 0.0f });

		//// For external use
		//void SetLevelTag(EntityID entityID, int levelTag);
		//void RemoveLevelTag(EntityID entityID);

	private:
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
		void ApplyThrust(EntityID id, float acceleration);
		void UpdateThrusterComponents(float dt);
		void UpdateBoosterComponents(float dt);
		void UpdateBrakeComponents();
		void UpdateProjectileLauncherComponents(float dt, bool secondaryLaunchers);
		void UpdateDrawAnimationComponents(float dt);
		void UpdateAIComponents(float dt);
		void ApplyImpulseDamage(Body* bodyPtr1, Body* bodyPtr2, const float* normalImpulses, unsigned numImpulses);

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
		void CreateExplosionFromEntity(EntityID entityID, const ParticleExplosionComponent& particleExplosion);

		// Box2D user data
		Body* GetUserBodyPtr(b2Body* b2BodyPtr) const;
		Body* GetUserBodyFromFixture(b2Fixture* fixturePtr);
		void SetB2BodyPtr(Body* bodyPtr, b2Body* b2BodyPtr);
		void SwapB2Bodies(Body& body1, Body& body2);

		// Graphics
		void DrawWorldEdge() const;
		void DrawLayer(int layer) const;
		void DrawParticleSystem(int layer) const;
		void DrawAllThrusterComponents(int layer) const;
		void DrawThrusterComponent(const ThrusterComponent& thrusterComponent, const b2Vec2& entitySize,
			const b2Vec2& position, float angle) const;
		void DrawAllAnimationComponents(int layer) const;
		void DrawAnimation(const d2d::Animation& animation, const b2Vec2& size, const b2Vec2& position, float angle) const;
		void DrawAllFixturesComponents(int layer) const;
		void DrawFixtureList(b2Fixture* fixturePtr, const b2Vec2& position, float angle, bool fill) const;
		void DrawAllHealthMeters() const;
		void DrawHealthMeter(float hp, float hpMax, const b2Vec2& position) const;
		void DrawRadar() const;

		//+---------------------------------------\
		//|         Private Data Types            |
		//\---------------------------------------/
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
			EntityID entityID;
			b2Vec2 position;
		};
		template<class T> class ComponentArray
			: public std::array<T, WORLD_MAX_ENTITIES>{};
		typedef std::array<CloneSyncData, WORLD_NUM_CLONES> CloneSyncDataArray;

		//+---------------------------------------\
		//|			    Private Data	          |
		//\---------------------------------------/
		WorldDef m_settings;
		EntityID m_highestActiveEntityCount{ 0 };
		DestroyListener* m_destructionListenerPtr{ nullptr };
		WrapListener* m_wrappedEntityListenerPtr{ nullptr };
		ProjectileLauncherListener* m_projectileLauncherListenerPtr{ nullptr };
		ExitListener* m_exitListenerPtr{ nullptr };

		float m_timestepAccumulator{ 0.0f };
		b2World* m_b2WorldPtr{ nullptr };
		std::set< EntityID > m_destroyBuffer;
		std::list< DamageData > m_damageDataList;
		b2Vec2 m_worldDimensions;
		b2Vec2 m_worldCenter;
		d2d::Rect m_worldRect;

		// All entities have these by default:
		ComponentArray< ComponentBitset > m_componentBits;
		ComponentArray< FlagBitset > m_flagBits;
		ComponentArray< bool > m_activeFlags;
		ComponentArray< b2Vec2 > m_sizeComponents;
		ComponentArray< float > m_boundingRadiusComponents;

		// These must be manually added after calling NewEntityID()
		//ComponentArray< int > m_levelTagComponents;
		ComponentArray< HealthComponent > m_healthComponents;
		ComponentArray< float > m_destructionDelayComponents;
		ComponentArray< float > m_destructionDelayOnContactComponents;
		ComponentArray< float > m_destructionChanceOnContactComponents;
		ComponentArray< RotatorComponent > m_rotatorComponents;
		ComponentArray< SetThrustFactorAfterDelayComponent > m_setThrustFactorAfterDelayComponents;
		ComponentArray< ThrusterComponent > m_thrusterComponents;
		ComponentArray< BoosterComponent > m_boosterComponents;
		ComponentArray< FuelComponent > m_fuelComponents;
		ComponentArray< BrakeComponent > m_brakeComponents;
		ComponentArray< ProjectileLauncherComponent > m_primaryProjectileLauncherComponents;
		ComponentArray< ProjectileLauncherComponent > m_secondaryProjectileLauncherComponents;
		ComponentArray< EntityID > m_parentComponents;

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
		ComponentArray< PowerUpComponent > m_powerUpComponents;
		ComponentArray< IconCollectorComponent > m_iconCollectorComponents;
		ComponentArray< AIComponent > m_AIComponents;
		ComponentArray< RadarComponent > m_radarComponents;

		d2d::ShapeFactory m_shapeFactory;
	};
}
