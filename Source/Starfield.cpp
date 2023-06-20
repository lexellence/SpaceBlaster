/**************************************************************************************\
** File: Starfield.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the Starfield class
**
\**************************************************************************************/
#include "pch.h"
#include "Starfield.h"

namespace Space
{
	void Starfield::Init(const StarfieldDef& def, const b2Vec2& cameraPosition)
	{
		m_cameraPosition = cameraPosition;
		m_pointSizeIndexRange = def.pointSizeIndexRange;
		{
			float paddedSize{ def.maxCameraDimension * def.edgePaddingFactor };
			m_boundaryRect.SetCenter(b2Vec2_zero, { paddedSize, paddedSize });
		}

		// Calculate number of stars and make sure it's not too many
		{
			float starfieldArea = m_boundaryRect.GetWidth() * m_boundaryRect.GetHeight();
			size_t numStars = starfieldArea * def.density;
			m_starList.resize(numStars);
		}

		// Random starfield
		for(Star& star : m_starList)
		{
			// Random position
			star.position = d2d::RandomVec2InRect(m_boundaryRect);

			// Speed/relativeSize/color properties are proportional to each other
			float randomPercent{ d2d::RandomFloatPercent() };
			float weightedRandomPercent{ powf(randomPercent, STARFIELD_SMALLER_STARS_WEIGHT_EXPONENT) };
			star.speedFactor = d2d::Lerp(def.speedFactorRange, weightedRandomPercent);
			{
				int pointSizeIndex = d2d::Lerp((float)m_pointSizeIndexRange.GetMin(), (float)m_pointSizeIndexRange.GetMax(), weightedRandomPercent) + 0.5f;
				int randomPointSizeIndexVariation{ d2d::RandomInt({-def.maxPointSizeIndexVariation, def.maxPointSizeIndexVariation}) };
				star.pointSizeIndex = d2d::GetClamped(pointSizeIndex + randomPointSizeIndexVariation, d2d::Window::VALID_POINT_SIZES);
			}

			// Color
			star.color = def.colorRange.Lerp(weightedRandomPercent);
			{
				float randomAlphaVariation = d2d::RandomFloat({ -def.maxAlphaVariation, def.maxAlphaVariation });
				star.color.alpha += randomAlphaVariation;
				d2d::Clamp(star.color.alpha, { 0.0f, 1.0f });
			}
		}
	}
	void Starfield::Update(const b2Vec2& cameraPosition)
	{
		b2Vec2 cameraChange{ cameraPosition - m_cameraPosition };
		for(Star& star : m_starList)
		{
			// Move star proportional to viewport change, then wrap around the boundary
			star.position -= star.speedFactor * cameraChange;
			d2d::WrapVector(star.position, m_boundaryRect);
		}
		m_cameraPosition = cameraPosition;
	}
	void Starfield::Draw() const
	{
		d2d::Window::PushMatrix();
		d2d::Window::Translate(m_cameraPosition);
		for(int sizeIndex = m_pointSizeIndexRange.GetMin(); sizeIndex <= m_pointSizeIndexRange.GetMax(); ++sizeIndex)
		{
			d2d::Window::SetPointSize(d2d::Window::POINT_SIZES[sizeIndex]);
			for(const Star& star : m_starList)
			{
				if(star.pointSizeIndex == sizeIndex)
				{
					d2d::Window::SetColor(star.color);
					d2d::Window::DrawPoint(star.position);
				}
			}
		}
		d2d::Window::PopMatrix();
	}
	void Starfield::Translate(const b2Vec2& translation)
	{
		m_cameraPosition += translation;
	}
}
