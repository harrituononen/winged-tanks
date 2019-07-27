#ifndef ENTITIES_TRAJECTORY_PROJECTILE_HPP
#define ENTITIES_TRAJECTORY_PROJECTILE_HPP

#include "projectile.hpp"
#include "../physics/rigid_body.hpp"
#include "../physics/rigid_body_with_collider.hpp"
#include "../physics/box_collider.hpp"

namespace entities {

class TrajectoryProjectile : public Projectile
{
private:
    physics::RigidBody m_rigid_body;

public:
    TrajectoryProjectile(graphics::Texture& tex, logic::GameplayState* game_state);

    ~TrajectoryProjectile() = default;

    TrajectoryProjectile(TrajectoryProjectile const&) = delete;
    TrajectoryProjectile& operator= (TrajectoryProjectile const&) = delete;

    math::Vector2f TrajectoryProjectile::check_collision(physics::BoxCollider& a) override;
    bool check_terrain_collision(float dt, entities::LevelTerrain& terr);
    void update(std::chrono::milliseconds const dt, LevelTerrain& terr) override;
    void resolve_collision(math::Vector2f const& coll, bool show_explosion) override;

    inline void set_position(math::Vector2f const& pos) override;
    inline void set_velocity(math::Vector2f const& vel) override;
};

} // namespace entities

#include "trajectory_projectile.inl"

#endif // ENTITIES_TRAJECTORY_PROJECTILE_HPP
