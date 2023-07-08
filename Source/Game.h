/**************************************************************************************\
** File: Game.h
** Project:
** Author: David Leksen
** Date:
**
** Header file for the Game class
**
\**************************************************************************************/
#pragma once
#include "World.h"
#include "Camera.h"
#include "Starfield.h"
#include "GameSettings.h"
#include "EntityFactory.h"
#include "ShopSettings.h"
namespace Space
{
	enum class GameAction
	{
		NEXT_LEVEL,
		RESTART_LEVEL
	};
	struct DelayedGameAction
	{
		GameAction action;
		float delay{};
		float timeElapsed{};
	};

	class Game
		: public DestroyListener,
		  public WrapListener,
		  public ProjectileLauncherListener,
		  public ExitListener
	{
	public:
		Game() = delete;
		Game(Camera* cameraPtr, Starfield* starfieldPtr);
		void NewGame();
		void Update(float dt, PlayerController &playerController);
		void Draw();
		bool DidPlayerExit() const;
		void StartCurrentLevel();
		float GetPlayerCredits() const;

		// Returns true if purchase was successful, otherwise returns false.
		bool PurchaseUpgrade(ShopItemID itemID, float price = 0.0f);

		// World callbacks
		void EntityWillBeDestroyed(EntityID entityID) override;
		void EntityWrapped(EntityID entityID, const b2Vec2 &translation) override;
		void ProjectileLaunched(const ProjectileDef& projectileDef, EntityID parentID) override;
		void EntityExited(EntityID entityID) override;

	private:
		void ClearLevel(const b2Vec2& newWorldDimensions);
		void ValidateWorldDimensions() const;

		void UpdateCamera(float dt, const PlayerController &playerController);
		void UpdateDelayedActions(float dt);
		void SetPlayer(EntityID entityID);
		bool IsPlayer(EntityID entityID) const;
		void ApplyPlayerUpgrades(World& world, EntityID playerID, const std::set<ShopItemID>& upgrades);
		void FollowEntity(EntityID entityID);
		void CreatePlayer(World &world, const InstanceDef &def);
		void DrawHUD();

	private:
		World m_world;
		EntityFactory m_factory;
		Camera *const m_cameraPtr;
		Starfield *const m_starfieldPtr;
		bool m_firstUpdate{ true };
		bool m_cameraFollowingEntity{ false };
		EntityID m_cameraFollowEntityID{};
		struct
		{
			bool isSet{};
			EntityID id{};
			float credits{};
			unsigned currentLevel{ 1 };
			bool exited{};
			std::set<ShopItemID> upgrades;
		} m_player;
		std::list<DelayedGameAction> m_delayedGameActions;
		d2d::FontReference m_hudFont{"Fonts/OrbitronLight.otf"};
	};
}
