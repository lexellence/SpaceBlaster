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
	const b2Vec2 STARFIELD_DEFAULT_VELOCITY{ -10.0f, -10.0f };
	const int STARFIELD_SMALLER_STARS_WEIGHT_EXPONENT = 1;
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
		void InitCameraPosition(const b2Vec2& cameraPosition);
		void Randomize();
		void Update(const b2Vec2& cameraPosition);
		void Draw() const;
		void MoveCameraWithoutMovingStars(const b2Vec2& translation);

	private:
		StarfieldDef m_def;
		b2Vec2 m_cameraPosition;
		d2d::Rect m_boundaryRect;
		struct Star
		{
			b2Vec2 position;
			float speedFactor;
			int pointSizeIndex;
			d2d::Color color;
		};
		std::vector<Star> m_starList;
	};
}
