/**************************************************************************************\
** File: Camera.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the Camera class
**
\**************************************************************************************/
#include "pch.h"
#include "Camera.h"
namespace Space
{
	void Camera::Init(const d2d::Range<float>& dimensionRange,
		float zoomSpeed, float initialZoomOutPercent,
		const b2Vec2& position)
	{
		m_dimensionRange = dimensionRange;
		m_zoomSpeed = zoomSpeed;
		m_initialZoomOutPercent = initialZoomOutPercent;
		m_zoomOutPercent = initialZoomOutPercent;
		m_position = position;
	}
	void Camera::Update(float dt, float zoomOutFactor)
	{
		d2d::ClampPercentSigned(zoomOutFactor);
		m_zoomOutPercent += dt * zoomOutFactor * m_zoomSpeed;
		d2d::ClampPercent(m_zoomOutPercent);
	}
	b2Vec2 Camera::GetDimensions(float xyAspectRatio) const
	{
		float zoomMaxSmallerDimension;
		b2Vec2 cameraDimensions;

		// If width is larger than height
		if(xyAspectRatio >= 1.0f)
		{
			// Use height as smaller dimension
			zoomMaxSmallerDimension = m_dimensionRange.GetMax() / xyAspectRatio;
			cameraDimensions.y = m_zoomOutPercent * (zoomMaxSmallerDimension - m_dimensionRange.GetMin()) + m_dimensionRange.GetMin();
			cameraDimensions.x = cameraDimensions.y * xyAspectRatio;
		}
		else
		{
			// Use width as smaller dimension
			zoomMaxSmallerDimension = m_dimensionRange.GetMax() * xyAspectRatio;
			cameraDimensions.x = m_zoomOutPercent * (zoomMaxSmallerDimension - m_dimensionRange.GetMin()) + m_dimensionRange.GetMin();
			cameraDimensions.y = cameraDimensions.x / xyAspectRatio;
		}
		return cameraDimensions;
	}
	const b2Vec2& Camera::GetPosition() const
	{
		return m_position;
	}
	void Camera::SetPosition(const b2Vec2& newPosition)
	{
		m_position = newPosition;
	}
}