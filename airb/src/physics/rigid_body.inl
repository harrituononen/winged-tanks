#include <cmath>

namespace physics
{

    inline void RigidBody::add_force(math::Vector2f const& f)
    {
        m_net_force += f;
    }

    inline void RigidBody::add_torque(float const t)
    {
        m_net_torque += t;
    }

    inline void RigidBody::reset_force()
    {
        m_net_force = math::Vector2f::zero();
    }

    inline void RigidBody::reset_torque()
    {
        m_net_torque = 0.0f;
    }

    inline void RigidBody::damp_angular_velocity(float const mod, std::chrono::milliseconds dt)
    {
        m_angular_velocity -= m_angular_velocity * mod * static_cast<float>(dt.count()) * 0.001f;
    }

    inline void RigidBody::set_mass(float const mass)
    {
        m_mass = mass;
    }

    inline void RigidBody::set_max_speed(float const max_spd)
    {
        m_max_speed = max_spd;
    }

    inline void RigidBody::set_position(math::Vector2f const& pos)
    {
        m_position = pos;
    }

    inline void RigidBody::set_velocity(math::Vector2f const& vel)
    {
        m_velocity = vel;
    }

    inline void RigidBody::set_max_angular_speed(float const max_ang_spd)
    {
        m_max_angular_speed = max_ang_spd;
    }

    inline math::Vector2f const& RigidBody::get_position() const
    {
        return m_position;
    }

    inline float RigidBody::get_rotation() const
    {
        return m_rotation;
    }

    inline void RigidBody::set_rotation(float const rotation)
    {
        m_rotation = rotation;
    }

    inline math::Vector2f const& RigidBody::get_velocity() const
    {
        return m_velocity;
    }

    inline math::Vector2f RigidBody::get_forward() const
    {
        return math::Vector2f({ cosf(m_rotation), sinf(m_rotation) });
    }

    inline float RigidBody::get_angular_velocity() const
    {
        return m_angular_velocity;
    }

    inline float RigidBody::get_inverse_mass() const
    {
        return m_inverse_mass;
    }

} // namespace physics
