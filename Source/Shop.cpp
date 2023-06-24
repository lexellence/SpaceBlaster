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
				for(ShopItemProgression& prog : room.itemProgressionList)
					for(auto itemIt = prog.begin(); itemIt != prog.end(); itemIt++)
						if(itemIt->id == id)
							itemIt = prog.erase(itemIt);
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
}