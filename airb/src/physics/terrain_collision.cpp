#include "terrain_collision.hpp"

namespace physics
{
TerrainCollision::TerrainCollision(math::Vector2f point, math::Vector2f normal, float correction)
    : m_collision_point(point)
    , m_collision_normal(normal)
    , m_correction(correction)

{}

}