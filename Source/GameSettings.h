/**************************************************************************************\
** File: GameConstants.h
** Project: 
** Author: David Leksen
** Date: 
**
** Header file for game constants
**
\**************************************************************************************/
#pragma once
namespace Space
{
	// General
	const int DEFAULT_DRAW_LAYER = 0;
	const float MIN_WORLD_TO_CAMERA_RATIO = 1.5f;
	const float LEVEL_CHANGE_DELAY = 3.0f;
	const float DEATH_PENALTY_CREDITS = 5.0f;

	// Relative entity heights
	const float SCOUT_TO_BLASTER_HEIGHT_RATIO = 58.0f / 109.0f;
	const float FAT_MISSILE_TO_MISSILE_HEIGHT_RATIO = 8.0f / 7.0f;
	const float LARGE_TO_XLARGE_ASTEROID_AVERAGE_HEIGHT_RATIO = 41.5f / 90.0f;
	const float MEDIUM_TO_XLARGE_ASTEROID_AVERAGE_HEIGHT_RATIO = 27.0f / 90.0f;
	const float SMALL_TO_XLARGE_ASTEROID_AVERAGE_HEIGHT_RATIO = 16.5f / 90.0f;

	// Rotation speed
	const float BLASTER_ROTATION_SPEED = 6.0f;
	const float SCOUT_ROTATION_SPEED = 6.0f;
	const float UFO_ROTATION_SPEED = 3.0f;

	// Thrusters
	const float BLASTER_THRUSTER_ACCELERATION = 8.0f;
	const float SCOUT_THRUSTER_ACCELERATION = 20.0f;
	const float BLASTER_BOOST_FACTOR = 8.0f;
	const float SCOUT_BOOST_FACTOR = 5.0f;
	const float BOOST_SECONDS = 0.2f;
	const float BOOST_COOLDOWN_SECONDS = 1.0f;

	// Thruster positions
	const float BLASTER_THRUSTER_OFFSET_X = -0.44f;
	const float SCOUT_THRUSTER_OFFSET_X = -0.9f;
	const float BLASTER_THRUSTER_INNER_SPREAD_Y = 0.33f;
	const float BLASTER_THRUSTER_OUTER_SPREAD_Y = 0.437f;
	const float SCOUT_THRUSTER_SPREAD_Y = 0.1f;

	// Thruster sizes
	const b2Vec2 BLASTER_THRUSTER_RELATIVE_SIZE{ 0.25f, 0.075f };
	const b2Vec2 BLASTER_BOOST_THRUSTER_RELATIVE_SIZE{ BLASTER_THRUSTER_RELATIVE_SIZE };
	const b2Vec2 SCOUT_THRUSTER_RELATIVE_SIZE{ 1.5f, 0.15f };

	// Fuel
	const float BLASTER_THRUSTER_FUEL_PER_SECOND = 0.02f * BLASTER_THRUSTER_ACCELERATION;
	const float SCOUT_THRUSTER_FUEL_PER_SECOND = 0.02f * SCOUT_THRUSTER_ACCELERATION;
	const float BLASTER_MAX_FUEL = 100.0f;
	const float SCOUT_MAX_FUEL = 25.0f;

	// Brakes
	const float BLASTER_BRAKE_DECELERATION = 40.0f;
	const float SCOUT_BRAKE_DECELERATION = 40.0f;

	// Projectile starting positions
	const float BLASTER_PROJECTILE_OFFSET_X = 0.1f;
	const float SCOUT_PROJECTILE_OFFSET_X = 0.1f;
	const float BLASTER_PROJECTILE_INNER_SPREAD_Y = 0.327f;
	const float BLASTER_PROJECTILE_OUTER_SPREAD_Y = 0.437f;
	const float SCOUT_PROJECTILE_SPREAD_Y = 0.437f;

	// Projectile launchers
	const float BLASTER_CANON_IMPULSE = 5.0f;
	const float SCOUT_CANON_IMPULSE = 4.0f;
	const float BLASTER_CANON_INTERVAL = 0.2f;
	const float SCOUT_CANON_INTERVAL = 0.4f;
	const float BLASTER_MISSILE_LAUNCHER_IMPULSE = 1.0f;
	const float BLASTER_MISSILE_LAUNCHER_INTERVAL = 0.3f;

	// Projectiles
	const bool BULLET_FIXED_ROTATION = false;
	const bool MISSILE_FIXED_ROTATION = false;
	const bool BULLET_CONTINUOUS_COLLISION_DETECTION = true;
	const bool MISSILE_CONTINUOUS_COLLISION_DETECTION = true;
	const bool BULLET_DESTRUCTION_DELAY = true;
	const bool MISSILE_DESTRUCTION_DELAY = true;
	const float BULLET_DESTRUCTION_DELAY_TIME = 10.0f;
	const float MISSILE_DESTRUCTION_DELAY_TIME = 10.0f;
	const bool BULLET_DESTRUCTION_DELAY_ON_CONTACT = true;
	const bool MISSILE_DESTRUCTION_DELAY_ON_CONTACT = true;
	const float BULLET_DESTRUCTION_DELAY_ON_CONTACT_TIME = 2.0f;
	const float MISSILE_DESTRUCTION_DELAY_ON_CONTACT_TIME = 2.0f;
	const bool BULLET_DESTRUCTION_CHANCE_ON_CONTACT = true;
	const bool MISSILE_DESTRUCTION_CHANCE_ON_CONTACT = true;
	const float BULLET_DESTRUCTION_CHANCE = 0.95f;
	const float MISSILE_DESTRUCTION_CHANCE = 1.0f;
	const bool BULLET_IGNORE_PARENT_COLLISIONS_UNTIL_FIRST_CONTACT = true;
	const bool MISSILE_IGNORE_PARENT_COLLISIONS_UNTIL_FIRST_CONTACT = true;
	const float BULLET_ACCELERATION = 0.0f;
	const float MISSILE_ACCELERATION = 20.0f;
	const float BULLET_ACCELERATION_TIME = 0.0f;
	const float MISSILE_ACCELERATION_TIME = 5.0f;

	// HP
	const float BLASTER_HP = 200.0f;
	const float SCOUT_HP = 30.0f;
	const float UFO_HP = 15.0f;
	const float XLARGE_ASTEROID_HP = 115.0f;
	const float LARGE_ASTEROID_HP = 65.0f;
	const float MEDIUM_ASTEROID_HP = 40.0f;
	const float SMALL_ASTEROID_HP = 25.0f;

	// Materials
	const d2d::Material SHIP_MATERIAL{ /*density*/ 1.0f, /*friction*/ 0.5f, /*restitution*/ 0.5f };
	const d2d::Material BULLET_MATERIAL{ /*density*/ 2.0f, /*friction*/ 0.9f, /*restitution*/ 0.2f };
	const d2d::Material MISSILE_MATERIAL{ /*density*/ 0.8f, /*friction*/ 0.7f, /*restitution*/ 0.7f };
	const d2d::Material ASTEROID_MATERIAL{ /*density*/ 1.5f, /*friction*/ 0.6f, /*restitution*/ 0.5f };
	const d2d::Material BUMPER_MATERIAL{ /*density*/ 20.0f, /*friction*/ 0.99f, /*restitution*/ 0.1f };
	const d2d::Material FUEL_MATERIAL{ /*density*/ 0.25f, /*friction*/ 0.6f, /*restitution*/ 0.5f };
	const d2d::Material ICON_MATERIAL{ /*density*/ 0.4f, /*friction*/ 0.6f, /*restitution*/ 0.5f };

	// Collision filters
	const d2d::Filter DEFAULT_FILTER{ d2d::FILTER_DEFAULT_CATEGORY_BITS, d2d::FILTER_DEFAULT_MASK_BITS, d2d::FILTER_DEFAULT_GROUP_INDEX };
	const d2d::Filter SHIP_FILTER{ DEFAULT_FILTER };
	const d2d::Filter BULLET_FILTER{ d2d::FILTER_DEFAULT_CATEGORY_BITS, d2d::FILTER_DEFAULT_MASK_BITS, -10 };
	const d2d::Filter MISSILE_FILTER{ d2d::FILTER_DEFAULT_CATEGORY_BITS, d2d::FILTER_DEFAULT_MASK_BITS, -10 };
	const d2d::Filter ASTEROID_FILTER{ DEFAULT_FILTER };
	const d2d::Filter BUMPER_FILTER{ DEFAULT_FILTER };
	const d2d::Filter FUEL_FILTER{ DEFAULT_FILTER };
	const d2d::Filter ICON_FILTER{ DEFAULT_FILTER };

	// Sizes
	const float BLASTER_HEIGHT = 4.0f;
	const float SCOUT_HEIGHT = BLASTER_HEIGHT * SCOUT_TO_BLASTER_HEIGHT_RATIO * 1.5f;
	const float UFO_HEIGHT = 3.0f;
	const float BULLET_HEIGHT = 0.35f;
	const float SMALL_BULLET_HEIGHT = 0.5f * BULLET_HEIGHT;
	const float MISSILE_HEIGHT = 0.5f;
	const float FAT_MISSILE_HEIGHT = FAT_MISSILE_TO_MISSILE_HEIGHT_RATIO * MISSILE_HEIGHT * 1.2f;
	const float MISSILE_TIME_PER_FRAME = 0.2f;
	const float XLARGE_ASTEROID_HEIGHT = 16.0f;
	const float LARGE_ASTEROID_HEIGHT = LARGE_TO_XLARGE_ASTEROID_AVERAGE_HEIGHT_RATIO * XLARGE_ASTEROID_HEIGHT;
	const float MEDIUM_ASTEROID_HEIGHT = MEDIUM_TO_XLARGE_ASTEROID_AVERAGE_HEIGHT_RATIO * XLARGE_ASTEROID_HEIGHT;
	const float SMALL_ASTEROID_HEIGHT = SMALL_TO_XLARGE_ASTEROID_AVERAGE_HEIGHT_RATIO * XLARGE_ASTEROID_HEIGHT;
	const float BUMPER_HEIGHT = 3.0f;
	const float SODA_HEIGHT = 3.0f;
	const float MELON_HEIGHT = 2.0f;
	const float APPLE_HEIGHT = 2.0f;
	const float ICON_HEIGHT = 4.0f;

	// Random entity placement
	const float MIN_BOUNDING_RADII_GAP_RELATIVE_TO_HEIGHT_ICONS = 2.0f;
	const float MIN_BOUNDING_RADII_GAP_RELATIVE_TO_HEIGHT = 0.25f;
	const unsigned MAX_ATTEMPTS_PER_ENTITY = 10000;

	// Asteroid spawning
	const d2d::Range<float> ASTEROID_STARTING_SPEED_RANGE_XL{ 0.1f, 3.0f };
	const d2d::Range<float> ASTEROID_STARTING_SPEED_RANGE_L{ 0.2f, 5.5f };
	const d2d::Range<float> ASTEROID_STARTING_SPEED_RANGE_M{ 0.3f, 8.5f };
	const d2d::Range<float> ASTEROID_STARTING_SPEED_RANGE_S{ 0.4f, 10.0f };

	const d2d::Range<float> ASTEROID_STARTING_ANG_VEL_RANGE_XL{ -0.8f, 0.8f };
	const d2d::Range<float> ASTEROID_STARTING_ANG_VEL_RANGE_L{ -2.0f, 2.0f };
	const d2d::Range<float> ASTEROID_STARTING_ANG_VEL_RANGE_M{ -3.25f, 3.25f };
	const d2d::Range<float> ASTEROID_STARTING_ANG_VEL_RANGE_S{ -5.5f, 5.5f };

	// Particle explosions - general
	const float PARTICLE_EXPLOSION_RELATIVE_SIZE = 1.0f;
	const float DAMAGE_BASED_SPEED_INCREASE_FACTOR = 0.3f;
	const float PARTICLE_EXPLOSION_FADEIN = 0.0f;

	// Particle explosions - num particles
	const unsigned BLASTER_NUM_PARTICLES = 150;
	const unsigned SCOUT_NUM_PARTICLES = 75;
	const unsigned UFO_NUM_PARTICLES = 15;
	const unsigned XLARGE_ASTEROID_NUM_PARTICLES = 80;
	const unsigned LARGE_ASTEROID_NUM_PARTICLES = 40;
	const unsigned MEDIUM_ASTEROID_NUM_PARTICLES = 20;
	const unsigned SMALL_ASTEROID_NUM_PARTICLES = 10;

	// Particle explosions - speed ranges
	const d2d::Range<float> BLASTER_PARTICLE_SPEED_RANGE{ 1.0f, 15.0f };
	const d2d::Range<float> SCOUT_PARTICLE_SPEED_RANGE{ 1.0f, 15.0f };
	const d2d::Range<float> UFO_PARTICLE_SPEED_RANGE{ 1.0f, 15.0f };
	const d2d::Range<float> ASTEROID_PARTICLE_SPEED_RANGE{ 1.0f, 15.0f };

	// Particle explosions - particle size ranges
	const d2d::Range<int> BLASTER_PARTICLE_SIZE_INDEX_RANGE{ 5, 14 };
	const d2d::Range<int> SCOUT_PARTICLE_SIZE_INDEX_RANGE{ 5, 14 };
	const d2d::Range<int> UFO_PARTICLE_SIZE_INDEX_RANGE{ 3, 12 };
	const d2d::Range<int> XLARGE_ASTEROID_PARTICLE_SIZE_INDEX_RANGE{ 6, 15 };
	const d2d::Range<int> LARGE_ASTEROID_PARTICLE_SIZE_INDEX_RANGE{ 5, 14 };
	const d2d::Range<int> MEDIUM_ASTEROID_PARTICLE_SIZE_INDEX_RANGE{ 4, 13 };
	const d2d::Range<int> SMALL_ASTEROID_PARTICLE_SIZE_INDEX_RANGE{ 3, 12 };

	// Particle explosions - particle color ranges
	const d2d::ColorRange BLASTER_PARTICLE_COLOR_RANGE{{39, 59, 39, 255}, { 87, 119, 87, 255 }};
	const d2d::ColorRange UFO_GRAY_PARTICLE_COLOR_RANGE{{99, 99, 99, 255}, { 219, 219, 219, 255 }};
	const d2d::ColorRange ASTEROID_PARTICLE_COLOR_RANGE{{92, 68, 52, 255}, { 153, 112, 85, 255 }};
	const d2d::ColorRange ROCK_PARTICLE_COLOR_RANGE{{115, 126, 131, 255}, { 154, 170, 177, 255 }};

	// Particle explosions - lifetimes
	const float BLASTER_PARTICLE_LIFETIME = 30.0f;
	const float SCOUT_PARTICLE_LIFETIME = 30.0f;
	const float UFO_PARTICLE_LIFETIME = 15.0f;
	const float XLARGE_ASTEROID_PARTICLE_LIFETIME = 10.0f;
	const float LARGE_ASTEROID_PARTICLE_LIFETIME = 8.0f;
	const float MEDIUM_ASTEROID_PARTICLE_LIFETIME = 6.0f;
	const float SMALL_ASTEROID_PARTICLE_LIFETIME = 4.0f;

	// Particle explosions - fadeout times
	const float BLASTER_PARTICLE_FADEOUT = 5.0f;
	const float SCOUT_PARTICLE_FADEOUT = 5.0f;
	const float XLARGE_ASTEROID_PARTICLE_FADEOUT = 4.0f;
	const float LARGE_ASTEROID_PARTICLE_FADEOUT = 3.0f;
	const float MEDIUM_ASTEROID_PARTICLE_FADEOUT = 2.0f;
	const float SMALL_ASTEROID_PARTICLE_FADEOUT = 1.0f;
}
