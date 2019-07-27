#ifndef PHYSICS_TERRAIN_COLLISION_HPP
#define PHYSICS_TERRAIN_COLLISION_HPP

#include "../math/matrix.hpp"

namespace physics
{
class TerrainCollision final
{
private:
    math::Vector2f m_collision_point;
    math::Vector2f m_collision_normal;
    float m_correction;

public:
    TerrainCollision(math::Vector2f point, math::Vector2f normal, float correction);
    ~TerrainCollision() = default;

    inline math::Vector2f const& get_collision_point() const;
    inline math::Vector2f const& get_collision_normal() const;
    inline float get_correction() const;

};
} // namespace physics

#include "terrain_collision.inl"

#endif // PHYSICS_TERRAIN_COLLISION_HPP
