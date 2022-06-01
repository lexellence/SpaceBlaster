/**************************************************************************************\
** File: ParticleSystem.h
** Project: 
** Author: David Leksen
** Date: 
**
** Header file for the ParticleSystem class
**
\**************************************************************************************/
#pragma once
namespace Space
{
	using ParticleID = unsigned long;
	struct ParticleTimer
	{
		float lifetime{};
		float fadeIn{};
		float fadeOut{};
		float age{};
	};
	struct ParticlePhysics
	{
		b2Vec2 lastPosition;
		b2Vec2 position;
		b2Vec2 velocity;
	};
	enum class ParticleExplosionType
	{
		CIRCLE, RECT, USE_BODY_SHAPES
	};
	const ParticleID MAX_PARTICLES{ 50000 };
	class ParticleSystem
	{
	public:
		~ParticleSystem();
		void Init();
		void Update(float dt);
		void SmoothStates(float timestepAlpha);
		float CalculateFadedAlpha(ParticleID index);

	private:
		// If you add more components, add it to CopyParticle()
		ParticleTimer timers[MAX_PARTICLES];
		ParticlePhysics physics[MAX_PARTICLES];
		b2Vec2 smoothedPositions[MAX_PARTICLES];
		int layers[MAX_PARTICLES];
		d2d::Color colors[MAX_PARTICLES];
		float fadedAlphas[MAX_PARTICLES];
		unsigned pointSizeIndices[MAX_PARTICLES];
		ParticleID firstUnusedIndex{ 0 };

		ParticleID m_highestActiveParticleCount{ 0u };
		float m_timestepAccumulator{ 0.0f };
		void CopyParticle(ParticleID from, ParticleID to);
		void DeleteParticle(ParticleID index);

		friend class World;
	};
}
