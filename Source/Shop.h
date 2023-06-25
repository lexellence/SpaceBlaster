/**************************************************************************************\
** File: Shop.h
** Project: 
** Author: David Leksen
** Date: 
**
** Header file for the Shop class
**
\**************************************************************************************/
#pragma once
namespace Space
{
	enum ShopItemID;
	struct ShopItem
	{
		std::string name;
		float price;
		ShopItemID id;
	};
	typedef std::list<ShopItem> ShopItemProgression;
	struct ShopRoom
	{
		std::string name;
		std::list<ShopItemProgression> itemProgressionList;
	};
	class Shop
	{
	public:
		void Init(const std::vector<ShopRoom>& roomList);
		void RemoveItems(const std::vector<ShopItemID>& itemIDs);
		std::vector<std::string> GetRoomNames() const;
		std::vector<std::string> GetShopItemNames(const std::string& roomName) const;
		std::vector<ShopItem> GetShopItems(const std::string& roomName) const;
	private:
		std::list<ShopItem> m_repairRoomItems;
		std::vector<ShopRoom> m_roomList;
	};
}
