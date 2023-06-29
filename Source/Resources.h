/**************************************************************************************\
** File: Resources.h
** Project:
** Author: David Leksen
** Date:
**
** Header file for resource loading
**
\**************************************************************************************/
#pragma once
namespace Space
{
	class ResourceManager
	{
	public:
		ResourceManager();
		struct
		{
			unsigned orbitronLight{};
			unsigned alexBrush{};
		} fonts;
		struct
		{
			struct
			{
				unsigned blaster;
				unsigned scout;
			} bigShips;
			struct
			{
				unsigned thruster;
			} effects;
			struct
			{
				unsigned bumper;
				unsigned apple;
				unsigned soda;
				unsigned melon;
			} items;
			struct
			{
				unsigned bullet;
				std::vector<unsigned> missileFrames;
				std::vector<unsigned> fatMissileFrames;
			} projectiles;
			std::vector<unsigned> icons;
			std::vector<unsigned> asteroidsXLarge;
			std::vector<unsigned> rocksXLarge;
			std::vector<unsigned> asteroidsLarge;
			std::vector<unsigned> rocksLarge;
			std::vector<unsigned> asteroidsMedium;
			std::vector<unsigned> rocksMedium;
			std::vector<unsigned> asteroidsSmall;
			std::vector<unsigned> rocksSmall;
		} textures;
		struct
		{

		} shapes;
	private:
		struct
		{
			unsigned bigShips;
			unsigned effects;
			unsigned items;
			unsigned projectiles;
			unsigned icons;
			unsigned asteroids;
		} m_atlases;
		void LoadFonts();
		void LoadTextures();
		std::vector<unsigned> LoadTextureListFromAtlas(unsigned atlasID, const std::vector<std::string>& filePaths);
	};
}
