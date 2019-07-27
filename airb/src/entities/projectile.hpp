#ifndef ENTITIES_PROJECTILE_HPP
#define ENTITIES_PROJECTILE_HPP

#include "../graphics/sprite.hpp"
#include "../graphics/texture.hpp"
#include "../math/matrix.hpp"
#include "../physics/rigid_body.hpp"
#include "../utilities/pool_object.hpp"

namespace logic {

class GameplayState;

} // namespace logic

namespace entities
{

class Projectile : public utilities::PoolObject
{
protected:
    static int const EXPLOSION_RADII[];

    int m_damage;
    int m_type;
    std::chrono::milliseconds m_lifetime;
    std::chrono::milliseconds m_elapsed_time;

    graphics::Sprite      m_sprite;
    logic::GameplayState* m_game_state;

public:
             Projectile(graphics::Texture& tex, logic::GameplayState* game_state);
    virtual ~Projectile() = default;

    Projectile            (Projectile const& projectile) = delete;
    Projectile& operator= (Projectile const& projectile) = delete;


    inline void set_damage   (int const dmg);
    inline void set_type     (int const type);
    inline int get_damage    ();

    virtual void set_position (math::Vector2f const& pos)          = 0;
    virtual void set_velocity (math::Vector2f const& vel)          = 0;
    virtual void update       (std::chrono::milliseconds const dt, entities::LevelTerrain& terr) = 0;
    virtual void resolve_collision(math::Vector2f const& coll, bool show_explosion) = 0;
    virtual math::Vector2f check_collision(physics::BoxCollider& a) = 0;

    inline void reset_elapsed_time    ();
    inline void set_lifetime          (std::chrono::milliseconds const& lifetime);
    inline void set_dimensions        (math::Vector2i const& dim);
    inline void set_source_dimensions (math::Vector2i const& src_dim);
    inline void set_source_position   (math::Vector2i const& src_pos);

    inline graphics::Sprite const& get_sprite() const;
};

} // namespace entities

#include "projectile.inl"

#endif // ENTITIES_PROJECTILE_HPP
