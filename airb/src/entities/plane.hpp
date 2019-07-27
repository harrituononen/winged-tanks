#ifndef ENTITIES_PLANE_HPP
#define ENTITIES_PLANE_HPP

#include <chrono>
#include <cstdlib>
#include <memory>

#include "consumable_resource.hpp"
#include "weapon.hpp"
#include "../graphics/sprite.hpp"
#include "../graphics/texture.hpp"
#include "../math/matrix.hpp"
#include "../physics/rigid_body_with_collider.hpp"
#include "../utilities/resource_holder.hpp"

namespace logic {

class GameplayState;

} // namespace logic

namespace entities {

class Plane
{
protected:
    static int const NUM_WEAPONS = 3;

    logic::GameplayState* m_game_state;
    physics::RigidBodyWithCollider    m_rigid_body;
    float                 m_thrust;
    float                 m_yaw;
    graphics::Sprite      m_sprite;

    int m_current_weapon;
    int m_previous_weapon;

    std::unique_ptr<Weapon> m_weapons[NUM_WEAPONS];

    entities::ConsumableResource<int> m_health;

public:
             Plane(float const mass,
                   float const max_spd, float const max_ang_spd,
                   std::vector<physics::BoxCollider> const& collider,
                   float const thrust, float const yaw,
                   utilities::ResourceHolder<graphics::Texture, graphics::Textures>& tex_hld,
                   logic::GameplayState* const g_st,
                   graphics::Texture& tex,
                   math::Vector2i const& src_pos, math::Vector2i const& src_dim,
                   math::Vector2i const& dim,
                   math::Vector2f const& pos);
    virtual ~Plane() = default;

    Plane            (Plane const&) = delete;
    Plane& operator= (Plane const&) = delete;

    void set_weapon (int const idx);
    void shoot      ();

    virtual void update(std::chrono::milliseconds const dt, LevelTerrain& terr);

    inline bool                     is_reloading      () const;
    inline physics::RigidBodyWithCollider&      get_rigid_body    ();
    inline graphics::Sprite&        get_weapon_sprite ();
    inline graphics::Sprite&        get_sprite        ();
    inline Weapon&                  get_weapon        ();
    inline int                      get_current_weapon() const;
    inline int                      get_ammo          () const;
    inline ConsumableResource<int>& get_health        ();

    inline void set_game_state      (logic::GameplayState* game_state);
    inline void set_weapon_rotation (float const rot);
    inline void take_damage         (int const damage);
    inline void reload_weapon       ();
    inline void switch_weapon       ();
};

} // namespace entities

#include "plane.inl"

#endif // ENTITIES_PLANE_HPP
