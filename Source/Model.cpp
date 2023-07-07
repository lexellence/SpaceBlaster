/**************************************************************************************\
** File: Model.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the Model class
**
\**************************************************************************************/
#include "pch.h"
#include "Model.h"
namespace Space
{
	Model::Model(std::string name, d2d::Texture* texturePtr)
		: name{ name },
		animationDef{ .frameList{ { texturePtr } } }
	{}
	Model::Model(std::string name, const d2d::AnimationDef& animationDef)
		: name{ name },
		animationDef{ animationDef }
	{}
}