#include "rigid_body_with_collider.hpp"
#include "../math/general.hpp"
#include "collision.hpp"

namespace physics
{
    RigidBodyWithCollider::RigidBodyWithCollider(float const mass,
                                                 float const max_spd,
                                                 float const max_ang_spd,
                                                 std::vector<BoxCollider> const& collider,
                                                 math::Vector2f const& pos)
        : RigidBody(mass, max_spd, max_ang_spd, pos)
        , m_collider(collider)
    {}

    void RigidBodyWithCollider::resolve_terrain_collision(math::Vector2f normal, math::Vector2f point, std::chrono::milliseconds const dt)
    {
        math::Vector2f rv = -m_velocity;
        math::Vector2f nv = normal.normalized();
        math::Vector2f vec = point - m_position;
        float xp = point[X] - m_position[X];
        float yp = point[Y] - m_position[Y];
        float cp = (vec[X] * nv[Y]) - (vec[Y] * nv[X]);
        float n = m_velocity.dot(nv);
        float impulse = m_inverse_mass + (abs(cp *
                                              cp)) / m_moment_of_inertia;

        float const sec = static_cast<float>(dt.count()) * 0.001f;
        m_velocity -= (2.0f * n) * nv;
        m_angular_velocity += (impulse / m_moment_of_inertia) * (xp*normal[Y] - yp*normal[X]);
    }


    void RigidBodyWithCollider::correct_positions(math::Vector2f corr, std::chrono::milliseconds const dt, RigidBodyWithCollider& b,
                                                  entities::LevelTerrain& terr)
    {
        float const sec = static_cast<float>(static_cast<double>(dt.count()) * 1e-3);
        const float percent = 0.2f;
        const float slop = 0.01f;
        math::Vector2f correction = std::max(corr.magnitude() - slop, 0.0f) / (m_inverse_mass + b.m_inverse_mass) * percent * corr.normalized();
        bool solid = 0;
        for (unsigned n = 0; n < m_collider.size(); ++n)
        {
            std::vector<math::Vector2f> vert = m_collider[n].get_vertexes();
            for (unsigned i = 0; i < vert.size(); ++i)
                vert[i] -= m_inverse_mass * correction;
            std::vector<int> alphas = { 1, 0, 2, 1, 3, 2, 0, 3 };
            for (int k = 0; k < 8; k += 2)
            {
                math::Vector2f move = vert[alphas[k]] - vert[alphas[k + 1]];
                int magn = (int)move.magnitude();
                math::Vector2f norm = move.normalized();
                for (int j = 0; j <= magn; ++j)
                {
                    math::Vector2i vec(vert[alphas[k + 1]] + (float)j * norm);
                    if (terr.is_pixel_solid(vec))
                    {
                        solid = 1;
                        break;
                    }
                }
                if (solid)
                    break;
            }
            if (solid)
                break;
        }
        if (!solid)
        {
            m_position -= m_inverse_mass * correction;
            for (unsigned i = 0; i < m_collider.size(); ++i)
            {
                m_collider[i].update_position(-m_inverse_mass * correction);
            }
        }
        solid = 0;
        for (unsigned n = 0; n < b.m_collider.size(); ++n)
        {
            std::vector<math::Vector2f> vert = b.m_collider[n].get_vertexes();
            for (unsigned i = 0; i < vert.size(); ++i)
                vert[i] += m_inverse_mass * correction;
            std::vector<int> alphas = { 1, 0, 2, 1, 3, 2, 0, 3 };
            for (int k = 0; k < 8; k += 2)
            {
                math::Vector2f move = vert[alphas[k]] - vert[alphas[k + 1]];
                int magn = (int)move.magnitude();
                math::Vector2f norm = move.normalized();
                for (int j = 0; j <= magn; ++j)
                {
                    math::Vector2i vec(vert[alphas[k + 1]] + (float)j * norm);
                    if (terr.is_pixel_solid(vec))
                    {
                        solid = 1;
                        break;
                    }
                }
                if (solid)
                    break;
            }
            if (solid)
                break;
        }
        if (!solid)
        {
            b.m_position += b.m_inverse_mass * correction;
            for (unsigned i = 0; i < m_collider.size(); ++i)
            {
                b.m_collider[i].update_position(b.m_inverse_mass * correction);
            }
        }

    }

    math::Vector2f RigidBodyWithCollider::find_point_of_impact(math::Vector2f normal)
    {
        math::Vector2f norm = math::Vector2f({ -normal[Y], normal[X] });
        for (int i = 0; i < 4; i++)
        {
            math::Vector2f vertex = m_collider[0].get_vertexes()[i];
            if (vertex[X] * norm[Y] - (vertex[Y] * norm[X]) == 0)
                return vertex;
        }
        return math::Vector2f::zero();
    }

    void RigidBodyWithCollider::resolve_collision(math::Vector2f normal, math::Vector2f point, RigidBodyWithCollider& b)
{
    math::Vector2f rv = b.m_velocity - m_velocity;
    float e = 1; //restitution
    float nv = rv.dot(normal);
    float j = -(1 + e) * nv;
    j /= m_inverse_mass + b.m_inverse_mass;

    math::Vector2f impulse = j * normal;
    m_velocity -= m_inverse_mass * impulse;
    b.m_velocity += b.m_inverse_mass * impulse;
    math::Vector2f ra = point - m_position;
    m_angular_velocity -= j*m_moment_of_inertia*((ra[X] * normal[Y]) - ra[Y] * normal[X]);
    math::Vector2f rb = point - b.m_position;
    b.m_angular_velocity -= j*b.m_moment_of_inertia*((rb[X] * normal[Y]) - rb[Y] * normal[X]);
}

    void RigidBodyWithCollider::update(std::chrono::milliseconds const dt, entities::LevelTerrain& terr)
    {
        float const sec = static_cast<float>(dt.count()) * 0.001f;

        m_acceleration = m_net_force / m_mass;
        m_angular_acceleration = m_net_torque / m_moment_of_inertia;

        m_velocity += m_acceleration * sec;
        float const vel_mag = m_velocity.magnitude();
        if (vel_mag != 0.0f)
            m_velocity = std::min(vel_mag, m_max_speed) * m_velocity.normalized();

        m_angular_velocity += m_angular_acceleration * sec;
        m_angular_velocity = m_angular_velocity < 0.0f ? std::max(m_angular_velocity,
                                                                  -m_max_angular_speed)
            : std::min(m_angular_velocity,
                       m_max_angular_speed);

        m_prev_position = m_position;

        std::vector<int> m_alphas = { 1, 0, 2, 1, 3, 2, 0, 3 };
        TerrainCollision coll = m_collider[0].check_terrain(*this, terr, dt, m_alphas);
        TerrainCollision coll2 = m_collider[1].check_terrain(*this, terr, dt, m_alphas);
        math::Vector2f normal = coll.get_collision_normal();
        math::Vector2f point = coll.get_collision_point();
        float correction = std::min(coll.get_correction(), coll2.get_correction());

        m_position += m_velocity         * sec * correction;
        m_prev_rotation = m_rotation;
        m_rotation += m_angular_velocity * sec * correction;

        float const s = m_rotation < -math::DOUBLE_PI ? 1.0f
            : m_rotation >  math::DOUBLE_PI ? -1.0f
            : 0.0f;

        m_rotation += s * math::DOUBLE_PI;

        for (unsigned i = 0; i < m_collider.size(); ++i)
        {
            m_collider[i].update_position(m_velocity * sec * correction);
            m_collider[i].update_rotation(m_rotation - m_prev_rotation, m_position);
        }
        if (coll.get_collision_normal() != math::Vector2f::zero() ||
            coll.get_collision_point() != math::Vector2f::zero() ||
            coll.get_correction() != 1.0f)
        {
            resolve_terrain_collision(coll.get_collision_normal(), coll.get_collision_point(), dt);
        }
        if (coll2.get_collision_normal() != math::Vector2f::zero() ||
            coll2.get_collision_point() != math::Vector2f::zero() ||
            coll2.get_correction() != 1.0f)
        {
            resolve_terrain_collision(coll2.get_collision_normal(), coll2.get_collision_point(), dt);
        }

        reset_force();
        reset_torque();
    }

} // namespace physics