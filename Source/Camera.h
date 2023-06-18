/**************************************************************************************\
** File: Camera.h
** Project:
** Author: David Leksen
** Date:
**
** Header file for the Camera class
**
\**************************************************************************************/
#pragma once
namespace Space
{
	class Camera
	{
	public:
		void Init(const d2d::Range<float>& dimensionRange,
			float zoomSpeed, float initialZoomOutPercent=0.5f,
			const b2Vec2& position=b2Vec2_zero);
		void Update(float dt, float zoomOutFactor);
		b2Vec2 GetDimensions(float xyAspectRatio) const;
		const d2d::Range<float>& GetDimensionRange() const;
		const b2Vec2& GetPosition() const;
		void SetPosition(const b2Vec2& position);
		d2d::Rect GetRect() const;

	private:
		d2d::Range<float> m_dimensionRange;
		float m_zoomSpeed{};
		float m_initialZoomOutPercent{};
		float m_zoomOutPercent{};
		b2Vec2 m_position;
	};
}
