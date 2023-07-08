/**************************************************************************************\
** File: WorldUtility.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for various World helper functions
**
\**************************************************************************************/
#include "pch.h"
#include "WorldUtility.h"
namespace Space
{
	float GetBoundingRadiiGap(const b2Vec2& position1, float radius1,
		const b2Vec2& position2, float radius2)
	{
		b2Vec2 relativePosition = position2 - position1;
		float combinedBoundingRadii = radius1 + radius2;
		return relativePosition.Length() - combinedBoundingRadii;
	}
}