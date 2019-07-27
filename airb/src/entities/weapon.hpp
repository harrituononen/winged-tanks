#ifndef ENTITIES_WEAPON_HPP
#define ENTITIES_WEAPON_HPP

#include <chrono>
#include <string>

#include "../graphics/sprite.hpp"
#include "../graphics/texture.hpp"
#include "../math/matrix.hpp"
#include "../physics/rigid_body.hpp"

namespace logic {

class GameplayState;

} // namespace logic

namespace entities {

class Plane;

class Weapon final
{
private:
    std::string const m_name;
    int               m_index;
    bool              m_automatic;
    int               m_rounds;
    int               m_mag_size;
    int               m_damage;
    float             m_bullet_speed;
    int               m_bullet_type;

    graphics::Sprite      m_sprite;
    logic::GameplayState* m_game_state;

    math::Vector2f m_origin;

    bool                      m_reloading;
    std::chrono::milliseconds m_fire_rate;
    std::chrono::milliseconds m_reload_time;
    std::chrono::milliseconds m_time_from_last_shot;
    std::chrono::milliseconds m_time_from_reload_start;
    std::chrono::milliseconds m_projectile_lifetime;

public:
     Weapon(Plane& p, graphics::Texture& tex, logic::GameplayState* game_state,
            std::string const& name, bool const automatic,
            std::chrono::milliseconds const fire_rate,
            std::chrono::milliseconds const reload_t,
            std::chrono::milliseconds const proj_lifetime,
            float const& bullet_speed, int const mag_size,
            int const damage,          int const bullet_type);
    ~Weapon() = default;

    Weapon            (Weapon const&) = delete;
    Weapon& operator= (Weapon const&) = delete;

    void update(std::chrono::milliseconds const dt);

    void shoot               ();
    void start_reloading     ();
    void stop_reloading      ();
    void set_barrel_position ();

    inline void set_position(math::Vector2f const& position);
    inline void set_rotation(float const rot);

    inline math::Vector2f get_turret_position () const;
    inline float          get_rotation        () const;

    inline std::string const&        get_name          () const;
    inline int                       get_index         () const;
    inline bool                      is_reloading      () const;
    inline bool                      is_automatic      () const;
    inline int                       get_rounds        () const;
    inline int                       get_magazine_size () const;
    inline std::chrono::milliseconds get_reload_time   () const;

    inline graphics::Sprite& get_sprite();
};

} // namespace entities

#include "weapon.inl"

#endif // ENTITIES_WEAPON_HPP
