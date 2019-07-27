#include "collision.hpp"

namespace physics
{
Collision::Collision(math::Vector2f mtv, int collider)
    : m_mtv(mtv)
    , m_collider(collider)
{ }
}