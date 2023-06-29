/**************************************************************************************\
** File: Resources.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for resource loading
**
\**************************************************************************************/
#include "pch.h"
#include "Resources.h"
namespace Space
{
	ResourceManager::ResourceManager()
	{
		d2d::Window::UnloadFonts();
		LoadFonts();

		d2d::Window::UnloadTextures();
		LoadTextures();
	}
	void ResourceManager::LoadFonts()
	{
		fonts.orbitronLight = d2d::Window::LoadFont("Fonts/OrbitronLight.otf");
		fonts.alexBrush = d2d::Window::LoadFont("Fonts/AlexBrush.otf");
	}
	void ResourceManager::LoadTextures()
	{
		std::string dir = "Textures/";

		// Big ships
		m_atlases.bigShips = d2d::Window::LoadTextureAtlas(dir + "bigships.png", dir + "bigships.xml");
		textures.bigShips.blaster = d2d::Window::LoadTextureFromAtlas(m_atlases.bigShips, "ship001");
		textures.bigShips.scout = d2d::Window::LoadTextureFromAtlas(m_atlases.bigShips, "ship002");

		// Effects
		m_atlases.effects = d2d::Window::LoadTextureAtlas(dir + "effects.png", dir + "effects.xml");
		textures.effects.thruster = d2d::Window::LoadTextureFromAtlas(m_atlases.effects, "thruster1");

		// Items
		m_atlases.items = d2d::Window::LoadTextureAtlas(dir + "items.png", dir + "items.xml");
		textures.items.bumper = d2d::Window::LoadTextureFromAtlas(m_atlases.items, "repulser1");
		textures.items.apple = d2d::Window::LoadTextureFromAtlas(m_atlases.items, "apple");
		textures.items.soda = d2d::Window::LoadTextureFromAtlas(m_atlases.items, "sodacan");
		textures.items.melon = d2d::Window::LoadTextureFromAtlas(m_atlases.items, "watermelon");

		// Projectiles
		m_atlases.projectiles = d2d::Window::LoadTextureAtlas(dir + "projectiles.png", dir + "projectiles.xml");
		textures.projectiles.bullet = d2d::Window::LoadTextureFromAtlas(m_atlases.projectiles, "fireball1");
		textures.projectiles.missileFrames = LoadTextureListFromAtlas(m_atlases.projectiles, { "rocket005a", "rocket005b" });
		textures.projectiles.fatMissileFrames = LoadTextureListFromAtlas(m_atlases.projectiles, { "rocket006a", "rocket006b" });

		// Icons
		m_atlases.icons = d2d::Window::LoadTextureAtlas(dir + "icons.png", dir + "icons.xml");
		textures.icons = LoadTextureListFromAtlas(m_atlases.icons,
			{
				"Solitaire", "RegistryDocument", "OutlookExpress", "NetworkSetup",
				"MyDocuments", "MyComputer", "Mouse", "MediaCenterFile",
				"GenericVideo", "GenericTextDocument", "GenericMedia", "GenericDocument",
				"GenericAudio", "GameController", "Font", "FolderClosed",
				"FloppyDisk", "EntireNetwork", "DVD-RW", "DLL",
				"DiskDefragmenter", "CommandPrompt", "ChangeOrRemovePrograms", "Calculator",
				"BluetoothDevices", "BAT", "IE6", "GIF"
			});

		// Asteroids
		m_atlases.asteroids = d2d::Window::LoadTextureAtlas(dir + "asteroids.png", dir + "asteroids.xml");
		textures.asteroidsXLarge = LoadTextureListFromAtlas(m_atlases.asteroids,
			{ "asteroidxlarge1", "asteroidxlarge2", "asteroidxlarge3", "asteroidxlarge4" });
		textures.rocksXLarge = LoadTextureListFromAtlas(m_atlases.asteroids,
			{ "rockxlarge1", "rockxlarge2", "rockxlarge3", "rockxlarge4" });
		textures.asteroidsLarge = LoadTextureListFromAtlas(m_atlases.asteroids, { "asteroidlarge1", "asteroidlarge2" });
		textures.rocksLarge = LoadTextureListFromAtlas(m_atlases.asteroids, { "rocklarge1", "rocklarge2" });
		textures.asteroidsMedium = LoadTextureListFromAtlas(m_atlases.asteroids, { "asteroidmedium1", "asteroidmedium2" });
		textures.rocksMedium = LoadTextureListFromAtlas(m_atlases.asteroids,	{ "rockmedium1", "rockmedium2" });
		textures.asteroidsSmall = LoadTextureListFromAtlas(m_atlases.asteroids, { "asteroidsmall1", "asteroidsmall2" });
		textures.rocksSmall = LoadTextureListFromAtlas(m_atlases.asteroids, { "rocksmall1", "rocksmall2" });
	}
	std::vector<unsigned> ResourceManager::LoadTextureListFromAtlas(unsigned atlasID, const std::vector<std::string>& filePaths)
	{
		std::vector<unsigned> frameListOut;
		for(const std::string& filePath : filePaths)
			frameListOut.push_back(d2d::Window::LoadTextureFromAtlas(atlasID, filePath));
		return frameListOut;
	}
}