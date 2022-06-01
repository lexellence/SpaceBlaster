/**************************************************************************************\
** File: ParticleSystem.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the ParticleSystem class
**
\**************************************************************************************/
#include "pch.h"
#include "ParticleSystem.h"

namespace Space
{
	ParticleSystem::~ParticleSystem()
	{
		d2LogDebug << "ParticleSystem required " << m_highestActiveParticleCount << " / " << MAX_PARTICLES << " particles. ";
	}
	void ParticleSystem::Init()
	{
		firstUnusedIndex = 0;
	}
	void ParticleSystem::Update(float dt)
	{
		// Keep track of how close we get to running out of particles
		if(firstUnusedIndex > m_highestActiveParticleCount)
			m_highestActiveParticleCount = firstUnusedIndex;

		// Save positions for interpolation, move particles, and interpolate
		for(ParticleID i = 0; i < firstUnusedIndex; ++i)
		{
			physics[i].lastPosition = physics[i].position;
			physics[i].position += dt * physics[i].velocity;
		}

		// Update ages and delete dead particles
		ParticleID i = 0;
		while(i < firstUnusedIndex)
		{
			timers[i].age += dt;
			if(timers[i].age >= timers[i].lifetime)
				DeleteParticle(i);
			else
				++i;
		}
	}
	void ParticleSystem::SmoothStates(float timestepAlpha)
	{
		for(ParticleID i = 0; i < firstUnusedIndex; ++i)
			smoothedPositions[i] = d2d::Lerp(physics[i].lastPosition, physics[i].position, timestepAlpha);
	}
	float ParticleSystem::CalculateFadedAlpha(ParticleID index)
	{
		// Partial alpha during fade-in or fade-out stages
		if(timers[index].age < timers[index].fadeIn)
		{
			float fadeInPercent{ timers[index].age / timers[index].fadeIn };
			return colors[index].alpha * fadeInPercent;
		}
		else if(timers[index].age > timers[index].lifetime - timers[index].fadeOut)
		{
			float fadeOutPercent{ (timers[index].age - (timers[index].lifetime - timers[index].fadeOut)) / timers[index].fadeOut };
			return colors[index].alpha * (1.0f - fadeOutPercent);
		}
		else
			return colors[index].alpha;
	}
	void ParticleSystem::CopyParticle(ParticleID from, ParticleID to)
	{
		timers[to] = timers[from];
		physics[to] = physics[from];
		layers[to] = layers[from];
		colors[to] = colors[from];
		pointSizeIndices[to] = pointSizeIndices[from];
	}
	void ParticleSystem::DeleteParticle(ParticleID index)
	{
		// Cut and paste particle from the end
		--firstUnusedIndex;
		CopyParticle(firstUnusedIndex, index);
	}
}


