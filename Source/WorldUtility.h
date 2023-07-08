/**************************************************************************************\
** File: WorldUtility.h
** Project: 
** Author: David Leksen
** Date: 
**
** Header file for various World helper functions
**
\**************************************************************************************/
#pragma once
namespace Space
{
	float GetBoundingRadiiGap(const b2Vec2& position1, float radius1,
		const b2Vec2& position2, float radius2);
}
