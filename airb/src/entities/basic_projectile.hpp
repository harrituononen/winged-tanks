#ifndef ENTITIES_BASIC_PROJECTILE_HPP
#define ENTITIES_BASIC_PROJECTILE_HPP

#include "projectile.hpp"
#include "../graphics/sprite.hpp"
#include "../graphics/texture.hpp"
#include "../math/matrix.hpp"

namespace entities
{

class BasicProjectile final : public Projectile
{
private:
    math::Vector2f m_position;
    math::Vector2f m_velocity;

public:
    BasicProjectile(graphics::Texture& tex, logic::GameplayState* game_state);

    ~BasicProjectile() = default;

    BasicProjectile(BasicProjectile const&) = delete;
    BasicProjectile& operator= (BasicProjectile const&) = delete;

    math::Vector2f check_collision(physics::BoxCollider& player) override;
    bool check_terrain_collision(float dt, entities::LevelTerrain& terr);
    void resolve_collision(math::Vector2f const& coll, bool show_explosion) override;
    void update(std::chrono::milliseconds const dt, LevelTerrain& terr) override;

    inline void set_position(math::Vector2f const& pos) override;
    inline void set_velocity(math::Vector2f const& vel) override;
};

} // namespace entities

#include "basic_projectile.inl"

#endif // ENTITIES_BASIC_PROJECTILE_HPP
