#ifndef PHYSICS_COLLISION_HPP
#define PHYSICS_COLLISION_HPP

#include "../math/matrix.hpp"
#include "box_collider.hpp"
#include "rigid_body_with_collider.hpp"

namespace physics
{

class Collision final
{
private:
    math::Vector2f m_mtv;
    int m_collider;
    
public:
    Collision(math::Vector2f mtv, int collider);
    ~Collision() = default;

    inline math::Vector2f get_mtv();
    inline int get_collider();
    inline void set_mtv(math::Vector2f mtv);
    inline void set_collider(int collider);
};

}   // namespace physics

#include "collision.inl"

#endif // PHYSICS_COLLISION_HPP