#ifndef PHYSICS_BOX_COLLIDER_HPP
#define PHYSICS_BOX_COLLIDER_HPP

#include <array>
#include <vector>
#include <chrono>

#include "../math/matrix.hpp"
#include "../entities/level_terrain.hpp"
#include "terrain_collision.hpp"

namespace physics {
class Collision;
class RigidBodyWithCollider;

class BoxCollider final
{
private:
    std::vector<math::Vector2f> m_vertexes;
    std::vector<math::Vector2f> m_prev_vertexes;
    std::vector<math::Vector2f> m_axes;
    math::Vector2f m_center;
    int m_size;
    int m_rb;
public:
    BoxCollider(std::vector<math::Vector2f> const& vertexes);
    ~BoxCollider() = default;

    BoxCollider(BoxCollider const& bc);
    BoxCollider& operator=(BoxCollider const& rhs);

    float signum(float n);
    Collision check_collision(BoxCollider& b);
    TerrainCollision check_terrain(RigidBodyWithCollider const& rb, entities::LevelTerrain& terr,
                       std::chrono::milliseconds const dt, std::vector<int> m_alphas); 
    void revert();
    void update_position(math::Vector2f const& change);
    void set_position(math::Vector2f const& pos, math::Vector2f const& offset_tl, 
                      math::Vector2f const& offset_tr, math::Vector2f const& offset_br,
                      math::Vector2f const& offset_bl);
    void update_rotation(float rotation, math::Vector2f point);
    inline std::vector<math::Vector2f> get_vertexes();
    inline math::Vector2f get_center();
    inline std::vector<math::Vector2f> get_axes();
    //inline RigidBodyWithCollider& get_rb();
    //inline void set_rb(RigidBodyWithCollider const& rb);
};

} // namespace physics

#include "box_collider.inl"

#endif // PHYSICS_BOX_COLLIDER_HPP
