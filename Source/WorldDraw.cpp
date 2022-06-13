/**************************************************************************************\
** File: WorldDraw.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for draw functions of the World class 
**
\**************************************************************************************/
#include "pch.h"
#include "World.h"
#include "ParticleSystem.h"

namespace Space
{
	void World::Draw()
	{
		DrawWorldEdge();
		for(int i = m_settings.drawLayerRange.GetMin(); i <= m_settings.drawLayerRange.GetMax(); ++i)
			DrawLayer(i);
		DrawAllHealthMeters();
		DrawRadar();
	}
	void World::DrawWorldEdge()
	{
		d2d::Window::SetColor({ 1.0f, 0.0f, 1.0f, 1.0f });
		d2d::Window::DrawRect(m_worldRect, false);

		std::array<CloneSection, 4> cloneLocations{ 
			CloneSection::TOP, CloneSection::RIGHT, 
			CloneSection::BOTTOM, CloneSection::LEFT };
		for(auto cloneLocation : cloneLocations)
		{
			b2Vec2 offset{ GetCloneOffset(cloneLocation) };
			d2d::Window::PushMatrix();
			d2d::Window::Translate(offset);
			d2d::Window::DrawRect(m_worldRect, false);
			d2d::Window::PopMatrix();
		}
	}
	void World::DrawLayer(int layer)
	{
		DrawParticleSystem(layer);
		DrawAllThrusterComponents(layer);
		DrawAllAnimationComponents(layer);
		DrawAllFixturesComponents(layer);
	}
	void World::DrawParticleSystem(int layer)
	{
		// One glPointSize call and one whole iteration through particle list for each point relativeSize
		d2d::Window::DisableTextures();
		d2d::Window::EnableBlending();
		for(unsigned sizeIndex = 0; sizeIndex < d2d::Window::NUM_POINT_SIZES; ++sizeIndex)
		{
			d2d::Window::SetPointSize(d2d::Window::POINT_SIZES[sizeIndex]);
			for(ParticleID i = 0; i < m_particleSystem.firstUnusedIndex; ++i)
				if(m_particleSystem.layers[i] == layer)
					if(m_particleSystem.pointSizeIndices[i] == sizeIndex)
					{
						d2d::Color newColor{ m_particleSystem.colors[i] };
						newColor.alpha = m_particleSystem.CalculateFadedAlpha(i);
						d2d::Window::SetColor(newColor);

						d2d::Window::DrawPoint(m_particleSystem.smoothedPositions[i]);
						CloneSectionList cloneLocations{ GetCloneSectionList(m_particleSystem.smoothedPositions[i]) };
						for(auto &cloneLocation : cloneLocations)
							d2d::Window::DrawPoint(m_particleSystem.smoothedPositions[i] + GetCloneOffset(cloneLocation));
					}
		}
	}
	void World::DrawAllThrusterComponents(int layer)
	{
		d2d::Window::EnableTextures();
		d2d::Window::EnableBlending();
		BitMask requiredComponents{ COMPONENT_THRUSTER | COMPONENT_PHYSICS };
		for(WorldID id = 0; id < WORLD_MAX_ENTITIES; ++id)
			if(m_drawAnimationComponents[id].layer == layer)
				if(HasComponents(id, requiredComponents) && HasSize2D(id) && IsActive(id))
					if(m_thrusterComponents[id].factor > 0.0f)
					{
						float angle{ m_smoothedTransforms[id].q.GetAngle() };
						DrawThrusterComponent(m_thrusterComponents[id], m_sizeComponents[id], m_smoothedTransforms[id].p, angle);
						for(const CloneBody& cloneBody : m_physicsComponents[id].cloneBodyList)
							DrawThrusterComponent(m_thrusterComponents[id], m_sizeComponents[id], m_smoothedTransforms[id].p + GetCloneOffset(cloneBody.section), angle);
					}
	}
	void World::DrawThrusterComponent(const ThrusterComponent& thrusterComponent, const b2Vec2& entitySize,	
							 const b2Vec2& position, float angle)
	{
		d2d::Window::PushMatrix();
		d2d::Window::Translate(position);
		d2d::Window::Rotate(angle);
		for(unsigned i = 0; i < thrusterComponent.numSlots; ++i)
		{
			const Thruster& thruster{ thrusterComponent.thrusters[i] };
			if(thruster.enabled && !thruster.temporarilyDisabled)
			{
				d2d::Color colorFactor{1.0f, 1.0f, 1.0f, thrusterComponent.factor };
				DrawAnimation(thruster.animation, entitySize, thruster.localRelativePosition * entitySize, 0.0f);
			}
		}
		d2d::Window::PopMatrix();
	}
	void World::DrawAllAnimationComponents(int layer)
	{
		d2d::Window::EnableTextures();
		d2d::Window::EnableBlending();
		BitMask requiredComponents{ COMPONENT_DRAW_ANIMATION | COMPONENT_PHYSICS };
		for(WorldID id = 0; id < WORLD_MAX_ENTITIES; ++id)
			if(m_drawAnimationComponents[id].layer == layer)
				if(HasComponents(id, requiredComponents) && HasSize2D(id) && IsActive(id))
				{
					float angle{ m_physicsComponents[id].mainBody.b2BodyPtr->GetAngle() };

					// Main entity
					DrawAnimation(m_drawAnimationComponents[id].animation, m_sizeComponents[id], m_smoothedTransforms[id].p, angle);

					// Clones
					for(const CloneBody& cloneBody : m_physicsComponents[id].cloneBodyList)
						DrawAnimation(m_drawAnimationComponents[id].animation, m_sizeComponents[id], m_smoothedTransforms[id].p + GetCloneOffset(cloneBody.section), angle);
				}
	}
	void World::DrawAnimation(const d2d::Animation& animation, const b2Vec2& size, const b2Vec2& position, float angle)
	{
		d2d::Window::PushMatrix();
		d2d::Window::Translate(position);
		d2d::Window::Rotate(angle);
		animation.Draw(size);
		d2d::Window::PopMatrix();
	}
	void World::DrawAllFixturesComponents(int layer)
	{
		d2d::Window::DisableTextures();
		d2d::Window::EnableBlending();
		d2d::Window::SetLineWidth(m_settings.drawFixturesLineWidth);
		BitMask requiredComponents{ COMPONENT_DRAW_FIXTURES | COMPONENT_PHYSICS };
		for(WorldID id = 0; id < WORLD_MAX_ENTITIES; ++id)
			if(m_drawAnimationComponents[id].layer == layer)
			{
				bool draw{ false };
				if(HasPhysics(id) && IsActive(id))
				{
					if(HasComponents(id, COMPONENT_DRAW_FIXTURES))
					{
						d2d::Window::SetColor(m_drawFixtureComponents[id].color);
						draw = true;
					}
					else if(m_settings.debugDrawFixtures)
					{
						d2d::Window::SetColor(WORLD_DEBUG_DRAW_FIXTURES_COLOR);
						draw = true;
					}
				}
				if(draw)
				{
					float angle{ m_smoothedTransforms[id].q.GetAngle() };
					DrawFixtureList(m_physicsComponents[id].mainBody.b2BodyPtr->GetFixtureList(), m_smoothedTransforms[id].p, angle, m_drawFixtureComponents[id].fill);
					for(const CloneBody& cloneBody : m_physicsComponents[id].cloneBodyList)
						DrawFixtureList(cloneBody.b2BodyPtr->GetFixtureList(),
							m_smoothedTransforms[id].p + GetCloneOffset(cloneBody.section), angle, m_drawFixtureComponents[id].fill);
				}
			}
	}
	void World::DrawFixtureList(b2Fixture* fixturePtr, const b2Vec2& position, float angle, bool fill)
	{
		d2d::Window::PushMatrix();
		d2d::Window::Translate(position);
		d2d::Window::Rotate(angle);
		while(fixturePtr)
		{
			switch(fixturePtr->GetType())
			{
			case b2Shape::e_circle: {
				b2CircleShape* circlePtr{ (b2CircleShape*)fixturePtr->GetShape() };
				if(circlePtr)
					d2d::Window::DrawCircle(circlePtr->m_p, circlePtr->m_radius, fill);}
									break;
			case b2Shape::e_edge: {
				b2EdgeShape* edgePtr{ (b2EdgeShape*)fixturePtr->GetShape() };
				if(edgePtr)
					d2d::Window::DrawLine(edgePtr->m_vertex1, edgePtr->m_vertex2);}
								  break;
			case b2Shape::e_polygon: {
				b2PolygonShape* polygonPtr{ (b2PolygonShape*)fixturePtr->GetShape() };
				if(polygonPtr && polygonPtr->m_count > 0)
					d2d::Window::DrawPolygon(polygonPtr->m_vertices, polygonPtr->m_count, fill);}
									 break;
			case b2Shape::e_chain: {
				b2ChainShape* chainPtr{ (b2ChainShape*)fixturePtr->GetShape() };
				if(chainPtr && chainPtr->m_count > 0)
					d2d::Window::DrawLineStrip(chainPtr->m_vertices, chainPtr->m_count);}
								   break;
			default: break;
			}
			fixturePtr = fixturePtr->GetNext();
		}
		d2d::Window::PopMatrix();
	}
	void World::DrawAllHealthMeters()
	{
		d2d::Window::DisableTextures();
		d2d::Window::EnableBlending();
		BitMask requiredComponents{ COMPONENT_HEALTH | COMPONENT_PHYSICS };
		for(WorldID id = 0; id < WORLD_MAX_ENTITIES; ++id)
			if(HasComponents(id, requiredComponents) && IsActive(id))
				if(m_healthComponents[id].hp < m_healthComponents[id].hpMax)
				{
					float meterOffsetY{ -(0.5f * m_sizeComponents[id].y + m_settings.healthMeter.gap) };
					b2Vec2 centerOfMass{ b2Mul(m_smoothedTransforms[id], GetLocalCenterOfMass(id)) };
					b2Vec2 meterPosition{ centerOfMass.x, centerOfMass.y + meterOffsetY };
					DrawHealthMeter(m_healthComponents[id].hp, m_healthComponents[id].hpMax, meterPosition);
					for(const CloneBody& cloneBody : m_physicsComponents[id].cloneBodyList)
						DrawHealthMeter(m_healthComponents[id].hp, m_healthComponents[id].hpMax, meterPosition + GetCloneOffset(cloneBody.section));
				}
	}
	void World::DrawHealthMeter(float hp, float hpMax, const b2Vec2& position)
	{
		d2d::Window::PushMatrix();
		d2d::Window::Translate(position);
		if(hpMax <= 0.0f)
			return;
		d2d::Clamp(hp, { 0.0f, hpMax });

		// Draw background for meter
		d2d::Rect meterRect;
		meterRect.SetCenter(b2Vec2_zero, { m_settings.healthMeter.widthPerPoint * hpMax, m_settings.healthMeter.height });
		d2d::Window::SetColor(m_settings.healthMeter.backgroundColor);
		d2d::Window::DrawRect(meterRect, true);

		// Draw hp meter with hp-based relativeSize and color
		float hpPercent{ hp / hpMax };
		meterRect.upperBound.x = meterRect.lowerBound.x + hpPercent * meterRect.GetWidth();
		if(hpPercent > m_settings.healthMeter.damagedThreshold)
			d2d::Window::SetColor(m_settings.healthMeter.hardlyDamagedColor);
		else if(hpPercent > m_settings.healthMeter.badlyDamagedThreshold)
			d2d::Window::SetColor(m_settings.healthMeter.damagedColor);
		else
			d2d::Window::SetColor(m_settings.healthMeter.badlyDamagedColor);
		d2d::Window::DrawRect(meterRect, true);
		d2d::Window::PopMatrix();
	}
	void World::DrawRadar()
	{

	}
}
