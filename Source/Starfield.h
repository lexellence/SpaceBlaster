/**************************************************************************************\
** File: Starfield.h
** Project:
** Author: David Leksen
** Date:
**
** Header file for the Starfield class
**
\**************************************************************************************/
#pragma once
namespace Space
{
	using StarID = unsigned;
	const StarID STARFIELD_MAX_STARS{ 50000 };
	struct StarfieldDef
	{
		float maxCameraDimension;
		float edgePaddingFactor;
		float density;
		d2d::Range<float> speedFactorRange;
		d2d::Range<int> pointSizeIndexRange;
		int maxPointSizeIndexVariation;
		d2d::ColorRange colorRange;
		float maxAlphaVariation;
	};
	class Starfield
	{
	public:
		void Init(const StarfieldDef& def);
		void Draw(const b2Vec2& newCameraPosition);
		void Translate(const b2Vec2& translation);

	private:
		bool m_firstDraw{ true };
		b2Vec2 m_cameraPosition;
		d2d::Rect m_boundaryRect;
		StarID m_numStars{};
		b2Vec2 m_positions[STARFIELD_MAX_STARS];
		float m_speedFactors[STARFIELD_MAX_STARS];
		int m_pointSizeIndices[STARFIELD_MAX_STARS];
		d2d::Range<int> m_pointSizeIndexRange;
		d2d::Color m_colors[STARFIELD_MAX_STARS];
	};
}
