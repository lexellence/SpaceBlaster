/**************************************************************************************\
** File: Game.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the Game class
**
\**************************************************************************************/
#include "pch.h"
#include "Game.h"
#include "Exceptions.h"
#include "GUISettings.h"
#include "EntityFactory.h"
namespace Space
{
	Game::Game(Camera* cameraPtr, Starfield* starfieldPtr)
		: m_cameraPtr{ cameraPtr }, m_starfieldPtr{ starfieldPtr }
	{
		d2Assert(m_cameraPtr);
		d2Assert(m_starfieldPtr);

		m_world.SetDestructionListener(this);
		m_world.SetWrapListener(this);
		m_world.SetProjectileLauncherListener(this);
		m_world.SetExitListener(this);
	}

	//+-----------------\-----------------------------------------
	//|	    NewGame     |
	//\-----------------/-----------------------------------------
	void Game::NewGame()
	{
		m_player.credits = 100.0f;
		m_player.currentLevel = 1;
		m_player.upgrades.clear();
	}

	//+-----------------\-----------------------------------------
	//|	   ClearLevel   |
	//\-----------------/-----------------------------------------
	void Game::ClearLevel(const b2Vec2& newWorldDimensions)
	{
		m_starfieldPtr->Randomize();
		m_cameraPtr->ResetZoom();
		m_cameraFollowingEntity = false;
		m_player.isSet = false;
		m_player.exited = false;
		m_delayedGameActions.clear();
		m_firstUpdate = true;

		d2d::Rect worldRect;
		worldRect.SetCenter(b2Vec2_zero, newWorldDimensions);
		m_world.Init(worldRect);
	}

	//+--------------------------------\--------------------------
	//|	      StartCurrentLevel        |
	//\--------------------------------/--------------------------
	void Game::StartCurrentLevel()
	{
		ClearLevel({ 500.0f, 500.0f });
		ValidateWorldDimensions();
		try
		{
			m_factory.CreateExit(m_world, { .position{ 0.0f, -12.0f } });
			CreatePlayer(m_world, { .position{ b2Vec2_zero }, .angle{ d2d::PI_OVER_TWO } });
			auto id = m_factory.CreateUFOGray(m_world, { .position{ 12.0f, 12.0f } });
			m_factory.CreateUFOGray(m_world, { .position{ -12.0f, 12.0f } });

			m_factory.CreateRandomIcons(m_world, 8);
			{
				float directionAngle = d2d::RandomFloat({ 0.0f, d2d::TWO_PI });
				m_factory.CreateRandomXLargeAsteroids(m_world, 20, directionAngle);
				m_factory.CreateRandomLargeAsteroids(m_world, 30, directionAngle);
				m_factory.CreateRandomMediumAsteroids(m_world, 40, directionAngle);
				m_factory.CreateRandomSmallAsteroids(m_world, 50, directionAngle);
			}
		}
		catch(CouldNotPlaceEntityException e)
		{
			d2d::Window::ShowSimpleMessageBox(d2d::MessageBoxType::D2D_ERROR, "Error Starting Level",
				e.what() + ": Could not find space to spawn entity. Restarting level."s);
			StartCurrentLevel();
		}
	}

	//+-----------------------\-----------------------------------
	//|	  GetPlayerCredits    |
	//\-----------------------/-----------------------------------
	float Game::GetPlayerCredits() const
	{
		return m_player.credits;
	}

	//+-----------------------\-----------------------------------
	//|	   PurchaseUpgrade    |
	//\-----------------------/
	//	Returns true if purchase was successful,
	//	otherwise returns false.
	//------------------------------------------------------------
	bool Game::PurchaseUpgrade(ShopItemID itemID, float price)
	{
		if(m_player.upgrades.contains(itemID))
			return true;

		if(m_player.credits >= price)
		{
			m_player.upgrades.insert(itemID);
			m_player.credits -= price;
			if(m_player.isSet)
				ApplyPlayerUpgrades(m_world, m_player.id, { itemID });
			return true;
		}
		return false;
	}

	//+-----------------------\-----------------------------------
	//|	 ApplyPlayerUpgrades  |
	//\-----------------------/-----------------------------------
	void Game::ApplyPlayerUpgrades(World& world, WorldID playerID, const std::set<ShopItemID>& upgrades)
	{
		for(auto itemID : upgrades)
			switch(itemID)
			{
			case ShopItemID::GUNS_2:
				m_factory.AddBlasterGuns(m_world, m_player.id, 2);
				break;
			case ShopItemID::GUNS_3:
				m_factory.AddBlasterGuns(m_world, m_player.id, 3);
				break;
			case ShopItemID::GUNS_4:
				m_factory.AddBlasterGuns(m_world, m_player.id, 4);
				break;
			case ShopItemID::GUNS_5:
				m_factory.AddBlasterGuns(m_world, m_player.id, 5);
				break;
			}
	}

	//+-----------------\-----------------------------------------
	//|	 DidPlayerExit  |
	//\-----------------/-----------------------------------------
	bool Game::DidPlayerExit() const
	{
		return m_player.exited;
	}

	//+--------------------------------\--------------------------
	//|	   ValidateWorldDimensions     |
	//\--------------------------------/--------------------------
	// Make sure world is not too small
	void Game::ValidateWorldDimensions() const
	{
		// Limit world dimensions relative to maximum camera dimensions
		float width = m_world.GetWorldRect().GetWidth();
		float height = m_world.GetWorldRect().GetHeight();
		float max = m_cameraPtr->GetDimensionRange().GetMax();
		if(width < max * MIN_WORLD_TO_CAMERA_RATIO)
			throw GameException{ "World width(" + d2d::ToString(width) + ") too small compared to max camera width(" + d2d::ToString(max) };
		if(height < max * MIN_WORLD_TO_CAMERA_RATIO)
			throw GameException{ "World height(" + d2d::ToString(height) + ") too small compared to max camera height(" + d2d::ToString(max) };
	}

	//+---------------------------\-------------------------------
	//|	        SetPlayer		  |
	//\---------------------------/-------------------------------
	void Game::SetPlayer(WorldID entityID)
	{
		if(m_world.EntityExists(entityID))
		{
			// Remove player controller from existing player entity
			if(m_player.isSet)
				m_world.SetFlags(m_player.id, FLAG_PLAYER_CONTROLLED, false);

			// Set new player
			m_world.SetFlags(entityID, FLAG_PLAYER_CONTROLLED, true);
			m_player.id = entityID;
			m_player.isSet = true;
		}
	}

	//+---------------------------\-------------------------------
	//|	        IsPlayer		  |
	//\---------------------------/-------------------------------
	bool Game::IsPlayer(WorldID entityID) const
	{
		return (m_player.isSet && entityID == m_player.id);
	}

	//+---------------------------\-------------------------------
	//|	      FollowEntity		  |
	//\---------------------------/-------------------------------
	void Game::FollowEntity(WorldID entityID)
	{
		m_cameraFollowingEntity = true;
		m_cameraFollowEntityID = entityID;
	}

	//+---------------------------\-------------------------------
	//|		 CreatePlayer		  |
	//\---------------------------/-------------------------------
	void Game::CreatePlayer(World& world, const InstanceDef& def)
	{
		WorldID blasterID = m_factory.CreateBlaster(world, def);
		world.AddIconCollectorComponent(blasterID, &m_player.credits);
		SetPlayer(blasterID);
		ApplyPlayerUpgrades(world, blasterID, m_player.upgrades);
		FollowEntity(blasterID);
	}

	//+-------------\---------------------------------------------
	//|	  Update    |
	//\-------------/---------------------------------------------
	void Game::Update(float dt, PlayerController& playerController)
	{
		m_world.Update(dt, playerController);
		b2Vec2 lastCameraPosition = m_cameraPtr->GetPosition();
		UpdateCamera(dt, playerController);
		if(m_firstUpdate)
		{
			m_starfieldPtr->MoveCameraWithoutMovingStars(m_cameraPtr->GetPosition() - lastCameraPosition);
			m_firstUpdate = false;
		}
		m_starfieldPtr->Update(m_cameraPtr->GetPosition());
		UpdateDelayedActions(dt);
	}

	//+--------------------------\--------------------------------
	//|	  UpdateDelayedActions   | 
	//\--------------------------/--------------------------------
	void Game::UpdateDelayedActions(float dt)
	{
		for(DelayedGameAction d : m_delayedGameActions)
			d.timeElapsed += dt;

		bool startLevel = false;
		for(auto it = m_delayedGameActions.begin(); it != m_delayedGameActions.end(); it++)
		{
			it->timeElapsed += dt;
			if(it->timeElapsed >= it->delay)
			{
				switch(it->action)
				{
				case GameAction::NEXT_LEVEL:
					m_player.currentLevel++;
					startLevel = true;
					break;
				case GameAction::RESTART_LEVEL:
					startLevel = true;
					break;
				}
				it = m_delayedGameActions.erase(it);
			}
		}
		if(startLevel)
		{
			StartCurrentLevel();
		}
	}

	//+--------------------------\--------------------------------
	//|	      UpdateCamera		 | 
	//\--------------------------/--------------------------------
	void Game::UpdateCamera(float dt, const PlayerController& playerController)
	{
		if(m_cameraFollowingEntity)
			if(m_world.HasPhysics(m_cameraFollowEntityID))
				m_cameraPtr->SetPosition(b2Mul(m_world.GetSmoothedTransform(m_cameraFollowEntityID),
					m_world.GetLocalCenterOfMass(m_cameraFollowEntityID)));
		m_cameraPtr->Update(dt, playerController.zoomOutFactor);
	}

	//+--------------------------\--------------------------------
	//|	 EntityWillBeDestroyed   | override (DestroyListener)
	//\--------------------------/--------------------------------
	void Game::EntityWillBeDestroyed(WorldID entityID)
	{
		if(m_cameraFollowingEntity && entityID == m_cameraFollowEntityID)
			m_cameraFollowingEntity = false;

		if(IsPlayer(entityID))
		{
			if(!m_player.exited)
			{
				m_player.credits -= DEATH_PENALTY_CREDITS;
				//m_delayedGameActions.push_back({ .action{ GameAction::RESTART_LEVEL }, .delay{ LEVEL_CHANGE_DELAY } });
			}
			m_player.isSet = false;
		}
	}
	//+--------------------------\--------------------------------
	//|	      EntityWrapped      | override (WrapListener)
	//\--------------------------/--------------------------------
	void Game::EntityWrapped(WorldID entityID, const b2Vec2& translation)
	{
		if(m_cameraFollowingEntity && entityID == m_cameraFollowEntityID)
			m_starfieldPtr->MoveCameraWithoutMovingStars(translation);
	}
	//+--------------------------\--------------------------------
	//|	   ProjectileLaunched    | override (ProjectileLauncherListener)
	//\--------------------------/--------------------------------
	void Game::ProjectileLaunched(const ProjectileDef& projectileDef, WorldID parentID)
	{}
	//+--------------------------\--------------------------------
	//|	      EntityExited       | override (ExitListener)
	//\--------------------------/--------------------------------
	void Game::EntityExited(WorldID entityID)
	{
		if(IsPlayer(entityID))
		{
			m_player.exited = true;
			m_player.currentLevel++;
		}
	}

	//+-------------\---------------------------------------------
	//|	   Draw     |
	//\-------------/---------------------------------------------
	void Game::Draw()
	{
		//d2d::Window::SetViewRect(GUISettings::HUD::ViewSections::ACTION);
		d2d::Window::SetViewRect();
		d2d::Window::SetCameraRect(m_cameraPtr->GetRect());
		m_starfieldPtr->Draw();
		m_world.Draw();

		if(m_player.isSet)
			DrawHUD();
	}

	//+-------------\---------------------------------------------
	//|	  DrawHUD   |
	//\-------------/---------------------------------------------
	void Game::DrawHUD()
	{
		b2Vec2 screenSize{ d2d::Window::GetScreenSize() };
		int screenWidth, screenHeight;
		d2d::Window::GetScreenSize(&screenWidth, &screenHeight);

		d2d::Window::SetViewRect();
		d2d::Window::SetCameraRect({ b2Vec2_zero, screenSize });

		d2d::Window::DisableTextures();
		d2d::Window::EnableBlending();

		// Draw fuel
		if(m_world.HasComponents(m_player.id, COMPONENT_FUEL))
		{
			d2d::Window::SetColor(GUISettings::HUD::Text::Color::FUEL);
			d2d::Window::PushMatrix();
			d2d::Window::Translate(GUISettings::HUD::Text::Position::FUEL * screenSize);
			{
				int fuelInt = (int)(m_world.GetFuelLevel(m_player.id) + 0.5f);
				int maxFuelInt = (int)(m_world.GetMaxFuelLevel(m_player.id) + 0.5f);
				std::string fuelString = "Fuel\n" + d2d::ToString(fuelInt) + "/" + d2d::ToString(maxFuelInt);
				d2d::Window::DrawString(fuelString, GUISettings::HUD::Text::Size::FUEL * screenSize.y,
					m_hudFont, GUISettings::HUD::Text::Position::FUEL_ALIGNMENT);
			}
			d2d::Window::PopMatrix();
		}

		// Draw icons remaining
		if(m_world.HasComponents(m_player.id, COMPONENT_ICON_COLLECTOR))
		{
			d2d::Window::SetColor(GUISettings::HUD::Text::Color::ICONS);
			d2d::Window::PushMatrix();
			d2d::Window::Translate(GUISettings::HUD::Text::Position::ICONS * screenSize);
			{
				//unsigned iconsLeft = m_world.GetIconsCollected(m_player.id);
				//std::string iconsString = "Icons\nLeft\n" + d2d::ToString(iconsLeft);
				//d2d::Window::DrawString(iconsString, GUISettings::HUD::Text::Size::ICONS * screenSize.y,
				//	m_hudFont, GUISettings::HUD::Text::Position::ICONS_ALIGNMENT);
			}
			d2d::Window::PopMatrix();
		}

		// Draw credits
		d2d::Window::SetColor(GUISettings::HUD::Text::Color::CREDITS);
		d2d::Window::PushMatrix();
		d2d::Window::Translate(GUISettings::HUD::Text::Position::CREDITS * screenSize);
		{
			std::string creditsString = "Credits\n" + d2d::ToString(m_player.credits);
			d2d::Window::DrawString(creditsString, GUISettings::HUD::Text::Size::CREDITS * screenSize.y,
				m_hudFont, GUISettings::HUD::Text::Position::CREDITS_ALIGNMENT);
		}
		d2d::Window::PopMatrix();

		// Draw level
		d2d::Window::SetColor(GUISettings::HUD::Text::Color::LEVEL);
		d2d::Window::PushMatrix();
		d2d::Window::Translate(GUISettings::HUD::Text::Position::LEVEL * screenSize);
		{
			std::string levelString = "Wave\n" + d2d::ToString(m_player.currentLevel);
			d2d::Window::DrawString(levelString, GUISettings::HUD::Text::Size::LEVEL * screenSize.y,
				m_hudFont, GUISettings::HUD::Text::Position::LEVEL_ALIGNMENT);
		}
		d2d::Window::PopMatrix();
	}
}
