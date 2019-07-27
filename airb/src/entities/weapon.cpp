#include "weapon.hpp"

#include "plane.hpp"
#include "../logic/gameplay_state.hpp"

namespace entities {

namespace
{
    math::Vector2i const TURRET_TEXTURE_POSITION({ graphics::TURRET_OFFSET_X,
                                                   graphics::TURRET_OFFSET_Y });
    math::Vector2i const TURRET_TEXTURE_DIMENSIONS({ graphics::TURRET_DIMENSIONS,
                                                     graphics::TURRET_DIMENSIONS });
}

Weapon::Weapon(Plane& p, graphics::Texture& tex, logic::GameplayState* game_state,
               std::string const& name, bool const automatic,
               std::chrono::milliseconds const fire_rate,
               std::chrono::milliseconds const reload_t,
               std::chrono::milliseconds const proj_lifetime,
               float const& bullet_speed, int const mag_size,
               int const damage,          int const bullet_type)
    : m_name                (name)
    , m_index               (bullet_type)
    , m_automatic           (automatic)
    , m_rounds              (mag_size)
    , m_mag_size            (mag_size)
    , m_damage              (damage)
    , m_bullet_speed        (bullet_speed)
    , m_bullet_type         (bullet_type)
    , m_projectile_lifetime (proj_lifetime)

    , m_sprite     (tex,
                    TURRET_TEXTURE_POSITION
                    + bullet_type * math::Vector2i::right() * graphics::TURRET_DIMENSIONS,
                    TURRET_TEXTURE_DIMENSIONS,
                    math::Vector2i::zero(), p.get_sprite().get_dimensions(),
                    math::Vector2f({ 0.5f, 0.5f }))
    , m_game_state (game_state)

    , m_origin ()

    , m_reloading              (false)
    , m_fire_rate              (fire_rate)
    , m_reload_time            (reload_t)
    , m_time_from_last_shot    (0)
    , m_time_from_reload_start (0)
{
    set_barrel_position();
}

void Weapon::update(std::chrono::milliseconds const dt)
{
    m_time_from_last_shot    += dt;
    m_time_from_reload_start += dt;

    if (m_reloading && m_time_from_reload_start >= m_reload_time)
    {
        m_rounds = m_mag_size;
        m_reloading = false;
    }
}

void Weapon::shoot()
{
    if (m_rounds == 0)
    {
        start_reloading();
        return;
    }

    GLfloat angle = m_sprite.get_rotation();

    math::Vector2f barrel_pos({
        m_origin[X] * cosf(angle) - m_origin[Y] * sinf(angle),
        m_origin[X] * sinf(angle) + m_origin[Y] * cosf(angle),
    });

    if (m_rounds > 0 && !m_reloading && m_time_from_last_shot >= m_fire_rate)
    {
        float const rot = get_rotation();
        math::Vector2f const dir({ cosf(rot), sinf(rot) });
        m_game_state->add_projectile(
            get_turret_position() + barrel_pos, dir,
            m_projectile_lifetime, m_bullet_speed,
            m_bullet_type, m_damage
        );
        m_time_from_last_shot = std::chrono::milliseconds(0);
        --m_rounds;
    }
}

void Weapon::start_reloading()
{
    if (m_reloading || m_rounds == m_mag_size)
        return;

    m_reloading = true;
    m_time_from_reload_start = std::chrono::milliseconds(0);
}

void Weapon::stop_reloading()
{
    m_reloading = false;
    m_time_from_reload_start = std::chrono::milliseconds(0);
}

void Weapon::set_barrel_position()
{
    static float const BARREL_OFFSETS[] = { 110.0f, 95.0f, 90.0f };

    math::Vector2f dim(m_sprite.get_dimensions());
    math::Vector2f barrel_pos({
        dim[X] / 2.0f + BARREL_OFFSETS[m_bullet_type] * dim[X]
        / static_cast<float>(m_sprite.get_source_dimensions()[X]),
        dim[Y] / 2.0f
    });
    math::Vector2f anchor(m_sprite.get_anchor());

    math::Vector2f anchor_pos({
        dim[X] * anchor[X], dim[Y] * anchor[Y]
    });
    m_origin = math::Vector2f(barrel_pos - anchor_pos);
}

} // namespace entities
