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
		float starfieldArea{ m_boundaryRect.GetWidth() * m_boundaryRect.GetHeight() };
		m_numStars = d2d::GetClamped((StarID)(starfieldArea * def.density), { 0, STARFIELD_MAX_STARS });

		// Random starfield
		for(StarID i = 0; i < m_numStars; ++i)
		{
			// Random position
			m_positions[i] = d2d::RandomVec2InRect(m_boundaryRect);

			// Speed/relativeSize/color properties are proportional to each other
			float randomPercent{ d2d::RandomFloatPercent() };
			float weightedRandomPercent{ powf(randomPercent, 8) };
			m_speedFactors[i] = d2d::Lerp(def.speedFactorRange, weightedRandomPercent);
			m_pointSizeIndices[i] = (int)(d2d::Lerp((float)def.pointSizeIndexRange.GetMin(), (float)def.pointSizeIndexRange.GetMax(), weightedRandomPercent) + 0.5f);
			int randomPointSizeIndexVariation{ d2d::RandomInt({-def.maxPointSizeIndexVariation, def.maxPointSizeIndexVariation}) };
			m_pointSizeIndices[i] = d2d::GetClamped(m_pointSizeIndices[i] + randomPointSizeIndexVariation, d2d::Window::VALID_POINT_SIZES);
			m_colors[i] = def.colorRange.Lerp(weightedRandomPercent);

			float randomAlphaVariation = d2d::RandomFloat({ -def.maxAlphaVariation, def.maxAlphaVariation });
			m_colors[i].alpha = std::clamp(m_colors[i].alpha + randomAlphaVariation, 0.0f, 1.0f);
		}
	}
	void Starfield::Update(const b2Vec2& cameraPosition)
	{
		b2Vec2 cameraChange{ cameraPosition - m_cameraPosition };
		for(StarID i = 0; i < m_numStars; ++i)
		{
			// Move star proportional to viewport change, then wrap around the boundary
			m_positions[i] -= m_speedFactors[i] * cameraChange;
			d2d::WrapVector(m_positions[i], m_boundaryRect);
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
			for(StarID i = 0; i < m_numStars; ++i)
			{
				if(m_pointSizeIndices[i] == sizeIndex)
				{
					d2d::Window::SetColor(m_colors[i]);
					d2d::Window::DrawPoint(m_positions[i]);
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
