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
	void Starfield::Init(const StarfieldDef& def)
	{
		m_firstDraw = true;
		m_cameraPosition.SetZero();
		m_pointSizeIndexRange = def.pointSizeIndexRange;
		{
			float paddedSize{ def.maxCameraDimension * def.edgePaddingFactor };
			m_boundaryRect.SetCenter(b2Vec2_zero, { paddedSize, paddedSize });
		}

		// Calculate number of stars and make sure it's not too many
		float starfieldArea{ m_boundaryRect.GetWidth() * m_boundaryRect.GetHeight() };
		m_numStars = d2d::GetClamped((unsigned)(starfieldArea * def.density), { 0u, STARFIELD_MAX_STARS });

		// Random starfield
		for (unsigned i = 0; i < m_numStars; ++i)
		{
			// Random position
			m_positions[i] = d2d::RandomVec2InRect(m_boundaryRect);

			// Speed/relativeSize/color properties are proportional to each other
			float randomPercent{ d2d::RandomFloatPercent() };
			float weightedRandomPercent{ pow(randomPercent, 8) };
			m_speedFactors[i] = d2d::Lerp(def.speedFactorRange, weightedRandomPercent);
			m_pointSizeIndices[i] = (int)(d2d::Lerp((float)def.pointSizeIndexRange.GetMin(), (float)def.pointSizeIndexRange.GetMax(), weightedRandomPercent) + 0.5f);
			int randomPointSizeIndexVariation{ d2d::RandomInt( {-def.maxPointSizeIndexVariation, def.maxPointSizeIndexVariation} ) };
			m_pointSizeIndices[i] = d2d::GetClamped(m_pointSizeIndices[i] + randomPointSizeIndexVariation, d2d::Window::VALID_POINT_SIZE_RANGE);
			m_colors[i] = def.colorRange.Lerp(weightedRandomPercent);

			float randomAlphaVariation = d2d::RandomFloat( {-def.maxAlphaVariation, def.maxAlphaVariation } );
			m_colors[i].alpha = std::clamp(m_colors[i].alpha + randomAlphaVariation, 0.0f, 1.0f);
		}
	}
	void Starfield::Draw(const b2Vec2& newCameraPosition)
	{
		// If camera moved, update star positions
		if(!m_firstDraw && newCameraPosition != m_cameraPosition)
		{
			b2Vec2 cameraChange{ newCameraPosition - m_cameraPosition };
			for(unsigned i = 0; i < m_numStars; ++i)
			{
				// Move star proportional to viewport change, then wrap around the boundary
				m_positions[i] -= m_speedFactors[i] * cameraChange;
				m_positions[i] = d2d::WrapVec2InRect(m_positions[i], m_boundaryRect);
			}
			m_cameraPosition = newCameraPosition;
		}

		// Draw stars
		d2d::Window::PushMatrix();
		d2d::Window::Translate(m_cameraPosition);
		for (int sizeIndex = m_pointSizeIndexRange.GetMin(); sizeIndex <= m_pointSizeIndexRange.GetMax(); ++sizeIndex)
		{
			d2d::Window::SetPointSize(d2d::Window::POINT_SIZES[sizeIndex]);
			for (unsigned i = 0; i < m_numStars; ++i)
			{
				if (m_pointSizeIndices[i] == sizeIndex)
				{
					d2d::Window::SetColor(m_colors[i]);
					d2d::Window::DrawPoint(m_positions[i]);
				}
			}
		}
		d2d::Window::PopMatrix();
		m_firstDraw = false;
	}
	void Starfield::Translate(const b2Vec2& translation)
	{
		m_cameraPosition += translation;
	}
}