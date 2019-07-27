#ifndef PHYSICS_RIGID_BODY_WITH_COLLIDER_HPP
#define PHYSICS_RIGID_BODY_WITH_COLLIDER_HPP

#include "rigid_body.hpp"

namespace physics
{
    class RigidBodyWithCollider final : public RigidBody
    {
    private:
        std::vector<BoxCollider> m_collider;
    public:
        RigidBodyWithCollider(float const mass,
                              float const max_spd,
                              float const max_ang_spd,
                              std::vector<BoxCollider> const& collider,
                              math::Vector2f const& pos);
        ~RigidBodyWithCollider() = default;

        void resolve_collision(math::Vector2f normal, math::Vector2f point, RigidBodyWithCollider& b);
        void resolve_terrain_collision(math::Vector2f normal, math::Vector2f point, std::chrono::milliseconds const dt);
        void correct_positions(math::Vector2f corr, std::chrono::milliseconds const dt, RigidBodyWithCollider& b, entities::LevelTerrain& terr);
        math::Vector2f find_point_of_impact(math::Vector2f normal);
        void update(std::chrono::milliseconds const dt, entities::LevelTerrain& terr) override;

        inline std::vector<BoxCollider>& get_collider();
    };

} // namespace physics

#include "rigid_body_with_collider.inl"

#endif // PHYSICS_RIGID_BODY_WITH_COLLIDER_HPP