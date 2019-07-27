#include "basic_projectile.hpp"
#include "../logic/gameplay_state.hpp"

namespace entities
{

BasicProjectile::BasicProjectile(graphics::Texture& tex, logic::GameplayState* game_state)
    : Projectile(tex, game_state)
{}

bool BasicProjectile::check_terrain_collision(float dt, entities::LevelTerrain& terr)
{
    float radius = m_sprite.get_dimensions()[X] / 2.0f;
    math::Vector2f norm_vel = m_velocity.normalized();
    math::Vector2f count = math::Vector2f::zero();
    while (abs(count[X]) < abs(dt * m_velocity[X]) || abs(count[Y]) < abs(dt * m_velocity[Y]))
    {
        math::Vector2i new_pos(m_position + count);
        if (new_pos[X] < 0 || new_pos[X] >= 6000 || new_pos[Y] < 0 || new_pos[Y] >= 4000)
        {
            deactivate();
            return true;
        }
        if (terr.is_pixel_solid(new_pos))
        {
            terr.destroy_circle(new_pos, EXPLOSION_RADII[m_type]);
            m_game_state->add_explosion(math::Vector2f(new_pos), m_type);
            deactivate();
            return true;
        }
        count += /*radius / 5 **/ norm_vel;
        if (abs(count[X]) > abs(dt * m_velocity[X]) || abs(count[Y]) > abs(dt * m_velocity[Y]))
        {
            count = m_velocity * dt;
            new_pos = math::Vector2i(m_position + count);
            if (new_pos[X] < 0 || new_pos[X] > 6000 || new_pos[Y] < 0 || new_pos[Y] > 4000)
            {
                deactivate();
                return true;
            }
            if (terr.is_pixel_solid(new_pos))
            {
                terr.destroy_circle(new_pos, EXPLOSION_RADII[m_type]);
                m_game_state->add_explosion(math::Vector2f(new_pos), m_type);
                deactivate();
                return true;
            }
            return false;
        }
        return false;
    }
    return false;
}

void BasicProjectile::resolve_collision(math::Vector2f const& coll, bool show_explosion)
{
    m_position += coll;
    if (show_explosion)
        m_game_state->add_explosion(m_position, m_type);
    deactivate();
}

math::Vector2f BasicProjectile::check_collision(physics::BoxCollider& a)
{
    std::vector<math::Vector2f> axes = { a.get_axes()[0], a.get_axes()[1] };
    math::Vector2f pos = m_position;
    float overlap = 10000;
    math::Vector2f smallest;
    float a_min, a_max, b_min, b_max;

    for (unsigned i = 0; i < axes.size(); i++)
    {

        a_min = a.get_vertexes()[0].dot(axes[i]);
        a_max = a.get_vertexes()[0].dot(axes[i]);
        b_min = m_position.dot(axes[i]);
        b_max = m_position.dot(axes[i]);
        for (unsigned j = 1; j < a.get_vertexes().size(); j++)
        {
            float n = a.get_vertexes()[j].dot(axes[i]);
            if (n < a_min)
                a_min = n;
            if (n > a_max)
                a_max = n;
        }

        if (b_min > a_max || b_max < a_min)
        {
            return math::Vector2f::zero();
        }
        else
        {
            float o = min(abs(a_max - b_max), abs(a_min - b_min));

            if (o < overlap)
            {
                overlap = o;
                smallest = axes[i];
            }
        }
    }
    return overlap * smallest;
}
void BasicProjectile::update(std::chrono::milliseconds const dt, LevelTerrain& terr)
{
    m_elapsed_time += dt;
    if (m_elapsed_time >= m_lifetime)
    {
        deactivate();
        m_game_state->add_explosion(m_position, m_type);
        terr.destroy_circle(math::Vector2i(m_position), EXPLOSION_RADII[m_type]);
        return;
    }

    float const sec = static_cast<float>(dt.count()) * 0.001f;

    if (!check_terrain_collision(sec, terr))
    {
        m_position += m_velocity * sec;
        m_sprite.set_position(math::Vector2i(m_position));
    }
}

} // namespace entities
