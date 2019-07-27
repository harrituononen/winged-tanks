#include "plane.hpp"

#include <algorithm>
#include <cassert>

#include "../graphics/texture.hpp"

namespace entities {

Plane::Plane(float const mass,
             float const max_spd, float const max_ang_spd,
             std::vector<physics::BoxCollider> const& collider,
             float const thrust, float const yaw,
             utilities::ResourceHolder<graphics::Texture, graphics::Textures>& tex_hld,
             logic::GameplayState* const g_st,
             graphics::Texture& tex,
             math::Vector2i const& src_pos, math::Vector2i const& src_dim,
             math::Vector2i const& dim,
             math::Vector2f const& pos)
    : m_game_state (g_st)
    , m_rigid_body (mass, max_spd, max_ang_spd, collider, pos)
    , m_thrust     (thrust)
    , m_yaw        (yaw)
    , m_sprite     (tex, src_pos, src_dim, math::Vector2i(pos), dim, math::Vector2f::one() * 0.5f)

    , m_current_weapon  (0)
    , m_previous_weapon (0)

    , m_weapons {
          std::make_unique<Weapon>(
              *this, tex_hld.get(graphics::Textures::GAMEPLAY_ELEMENTS), m_game_state,
              "Cannon", false,
              std::chrono::milliseconds(750), std::chrono::milliseconds(3000), std::chrono::milliseconds(2500),
              1250.0f, 5, 40, 0
          ),
          std::make_unique<Weapon>(
              *this, tex_hld.get(graphics::Textures::GAMEPLAY_ELEMENTS), m_game_state,
              "Syringe", false,
              std::chrono::milliseconds(400), std::chrono::milliseconds(1500), std::chrono::milliseconds(2000),
              1500.0f, 10, 10, 1
          ),
          std::make_unique<Weapon>(
              *this, tex_hld.get(graphics::Textures::GAMEPLAY_ELEMENTS), m_game_state,
              "Needler", true,
              std::chrono::milliseconds(100), std::chrono::milliseconds(1500), std::chrono::milliseconds(1500),
              1000.0f, 20, 5, 2
          )
      }

    , m_health (100)
{ }

void Plane::update(std::chrono::milliseconds const dt, LevelTerrain& terr)
{
    m_rigid_body.add_static_forces();
    m_rigid_body.update(dt, terr);
    auto& weap = m_weapons[m_current_weapon];
    weap->update(dt);

    math::Vector2f const& rb_pos = m_rigid_body.get_position();
    m_sprite.set_position(math::Vector2i(rb_pos));
    m_sprite.set_rotation(m_rigid_body.get_rotation());
    weap->set_position(rb_pos);
}

void Plane::set_weapon(int const idx)
{
    assert(idx >= 0);
    assert(idx < NUM_WEAPONS);

    m_weapons[m_current_weapon]->stop_reloading();
    m_previous_weapon = m_current_weapon; 
    m_current_weapon  = idx;
}

void Plane::shoot()
{
    m_weapons[m_current_weapon]->shoot();
}

} // namespace entities
