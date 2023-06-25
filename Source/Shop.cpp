/**************************************************************************************\
** File: Shop.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the Shop class
**
\**************************************************************************************/
#include "pch.h"
#include "Shop.h"
namespace Space
{
	void Shop::Init(const std::vector<ShopRoom>& roomList)
	{ 
		m_roomList = roomList;
	}
	void Shop::RemoveItems(const std::vector<ShopItemID>& itemIDs)
	{
		for(ShopItemID id : itemIDs)
			for(ShopRoom& room : m_roomList)
				for(auto progressionIt = room.itemProgressionList.begin(); 
					progressionIt != room.itemProgressionList.end(); progressionIt++)
				{
					for(auto itemIt = progressionIt->begin(); itemIt != progressionIt->end(); itemIt++)
						if(itemIt->id == id)
							itemIt = progressionIt->erase(itemIt);
					if(progressionIt->empty())
						progressionIt = room.itemProgressionList.erase(progressionIt);
				}
	}
	std::vector<std::string> Shop::GetRoomNames() const
	{
		std::vector<std::string> names;
		for(const ShopRoom& room : m_roomList)
			names.push_back(room.name);
		return names;
	}
	std::vector<std::string> Shop::GetShopItemNames(const std::string& roomName) const
	{
		std::vector<std::string> names;
		for(const ShopRoom& room : m_roomList)
			if(room.name == roomName)
			{
				for(const ShopItemProgression& prog : room.itemProgressionList)
					names.push_back(prog.front().name);
				return names;
			}
		return names;
	}
	std::vector<ShopItem> Shop::GetShopItems(const std::string& roomName) const
	{
		std::vector<ShopItem> items;
		for(const ShopRoom& room : m_roomList)
		{
			if(room.name == roomName)
			{
				for(const ShopItemProgression& prog : room.itemProgressionList)
					items.push_back(prog.front());
				return items;
			}
		}
		return items;
	}
}