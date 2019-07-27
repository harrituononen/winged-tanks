#include "rigid_body.hpp"

#include <algorithm>
#include <cassert>

#include "../math/general.hpp"

namespace physics
{

    RigidBody::RigidBody(float const mass,
                         float const max_spd,
                         float const max_ang_spd,
                         math::Vector2f const& pos,
                         float const rot,
                         math::Vector2f const& vel,
                         float const ang_vel,
                         math::Vector2f const& accel,
                         float const ang_accel,
                         math::Vector2f const& net_f,
                         float const net_t)
        : m_mass(mass)
        , m_inverse_mass(mass == 0.0f ? 0.0f : 1.0f / mass)
        , m_moment_of_inertia(mass / 6.0f)
        , m_max_speed(max_spd)
        , m_max_angular_speed(max_ang_spd)

        , m_position(pos)
        , m_rotation(rot)
        , m_velocity(vel)
        , m_angular_velocity(ang_vel)
        , m_acceleration(accel)
        , m_angular_acceleration(ang_accel)
        , m_net_force(net_f)
        , m_net_torque(net_t)
    {
        assert(mass != 0.0f);
    }

    RigidBody::RigidBody(RigidBody const& rb)
        : m_mass(rb.m_mass)
        , m_moment_of_inertia(rb.m_moment_of_inertia)
        , m_max_speed(rb.m_max_speed)
        , m_max_angular_speed(rb.m_max_angular_speed)

        , m_position(rb.m_position)
        , m_rotation(rb.m_rotation)
        , m_velocity(rb.m_velocity)
        , m_angular_velocity(rb.m_angular_velocity)
        , m_acceleration(rb.m_acceleration)
        , m_angular_acceleration(rb.m_angular_acceleration)
        , m_net_force(rb.m_net_force)
        , m_net_torque(rb.m_net_torque)
    {}

    RigidBody& RigidBody::operator=(RigidBody const& rhs)
    {
        m_mass = rhs.m_mass;
        m_moment_of_inertia = rhs.m_moment_of_inertia;
        m_max_speed = rhs.m_max_speed;
        m_max_angular_speed = rhs.m_max_angular_speed;

        m_position = rhs.m_position;
        m_rotation = rhs.m_rotation;
        m_velocity = rhs.m_velocity;
        m_angular_velocity = rhs.m_angular_velocity;
        m_acceleration = rhs.m_acceleration;
        m_angular_acceleration = rhs.m_angular_acceleration;
        m_net_force = rhs.m_net_force;
        m_net_torque = rhs.m_net_torque;

        return *this;
    }

    void RigidBody::update(std::chrono::milliseconds const dt, entities::LevelTerrain& terr)
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

        m_position += m_velocity         * sec;
        m_prev_rotation = m_rotation;
        m_rotation += m_angular_velocity * sec;

        float const s = m_rotation < -math::DOUBLE_PI ? 1.0f
            : m_rotation >  math::DOUBLE_PI ? -1.0f
            : 0.0f;
        m_rotation += s * math::DOUBLE_PI;

        reset_force();
        reset_torque();
    }


    void RigidBody::add_static_forces()
    {
        static math::Vector2f const GRAVITY = math::Vector2f::down() * 9.81f;
        static float const DRAG = 0.005f;

        m_net_force += m_mass * GRAVITY - DRAG * m_velocity.magnitude() * m_velocity;
    }

} // namespace physics
