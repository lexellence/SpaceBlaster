/**************************************************************************************\
** File: Model.h
** Project: 
** Author: David Leksen
** Date: 
**
** Header file for the Model class
**
\**************************************************************************************/
#pragma once
namespace Space
{
	struct Model
	{
		Model() = default;
		Model(std::string name, d2d::Texture* texturePtr);
		Model(std::string name, const d2d::AnimationDef& animationDef);
		std::string name;
		d2d::AnimationDef animationDef;
	};
}
