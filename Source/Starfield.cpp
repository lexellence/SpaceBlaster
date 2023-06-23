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
	void Starfield::Init(const StarfieldDef& def, float maxCameraDimension)
	{
		m_def = def;
		m_cameraPosition = b2Vec2_zero;
		{
			// Boundary
			float paddedSize{ maxCameraDimension * m_def.edgePaddingFactor };
			m_boundaryRect.SetCenter(b2Vec2_zero, { paddedSize, paddedSize });
		}
		{
			// Number of stars
			float starfieldArea = m_boundaryRect.GetWidth() * m_boundaryRect.GetHeight();
			size_t numStars = (size_t)(starfieldArea * m_def.density + 0.5f);
			m_starList.resize(numStars);
		}
		Randomize();
	}
	void Starfield::InitCameraPosition(const b2Vec2& cameraPosition)
	{
		m_cameraPosition = cameraPosition;
	}
	void Starfield::Randomize()
	{
		for(Star& star : m_starList)
		{
			// Random position
			star.position = d2d::RandomVec2InRect(m_boundaryRect);

			// Speed/relativeSize/color properties are proportional to each other
			float randomPercent{ d2d::RandomFloatPercent() };
			float weightedRandomPercent{ powf(randomPercent, STARFIELD_SMALLER_STARS_WEIGHT_EXPONENT) };
			star.speedFactor = d2d::Lerp(m_def.speedFactorRange, weightedRandomPercent);
			{
				int pointSizeIndex = (int)(d2d::Lerp((float)m_def.pointSizeIndexRange.GetMin(), (float)m_def.pointSizeIndexRange.GetMax(), weightedRandomPercent) + 0.5f);
				int randomPointSizeIndexVariation{ d2d::RandomInt({-m_def.maxPointSizeIndexVariation, m_def.maxPointSizeIndexVariation}) };
				star.pointSizeIndex = d2d::GetClamped(pointSizeIndex + randomPointSizeIndexVariation, d2d::Window::VALID_POINT_SIZES);
				d2d::Clamp(star.pointSizeIndex, m_def.pointSizeIndexRange);
			}
			// Color
			star.color = m_def.colorRange.Lerp(weightedRandomPercent);
			{
				float randomAlphaVariation = d2d::RandomFloat({ -m_def.maxAlphaVariation, m_def.maxAlphaVariation });
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
		d2d::Window::DisableTextures();
		d2d::Window::EnableBlending();

		d2d::Window::PushMatrix();
		d2d::Window::Translate(m_cameraPosition);
		for(int sizeIndex = m_def.pointSizeIndexRange.GetMin(); sizeIndex <= m_def.pointSizeIndexRange.GetMax(); ++sizeIndex)
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
	void Starfield::MoveCameraWithoutMovingStars(const b2Vec2& translation)
	{
		m_cameraPosition += translation;
	}
}
