#ifndef PHYSICS_RIGID_BODY_HPP
#define PHYSICS_RIGID_BODY_HPP

#include <chrono>

#include "../math/matrix.hpp"
#include "box_collider.hpp"

namespace physics
{

    class RigidBody
    {
    protected:
        float m_mass;
        float m_moment_of_inertia;
        float m_max_speed;
        float m_max_angular_speed;
        float m_inverse_mass;

        math::Vector2f m_position;
        float          m_rotation;
        math::Vector2f m_prev_position;
        float          m_prev_rotation;
        math::Vector2f m_velocity;
        float          m_angular_velocity;
        math::Vector2f m_acceleration;
        float          m_angular_acceleration;
        math::Vector2f m_net_force;
        float          m_net_torque;

    public:
        RigidBody(float const mass,
                  float const max_spd,
                  float const max_ang_spd,
                  math::Vector2f const& pos = math::Vector2f::zero(),
                  float const rot = 0.0f,
                  math::Vector2f const& vel = math::Vector2f::zero(),
                  float const ang_vel = 0.0f,
                  math::Vector2f const& accel = math::Vector2f::zero(),
                  float const ang_accel = 0.0f,
                  math::Vector2f const& net_f = math::Vector2f::zero(),
                  float const net_t = 0.0f);
        virtual ~RigidBody() = default;

        RigidBody(RigidBody const& rb);
        RigidBody& operator= (RigidBody const& rhs);

        virtual void update(std::chrono::milliseconds const dt, entities::LevelTerrain& terr);

        void add_static_forces();

        inline void add_force(math::Vector2f const& f);
        inline void add_torque(float const t);
        inline void reset_force();
        inline void reset_torque();

        inline void set_mass(float const mass);
        inline void set_max_speed(float const max_spd);
        inline void set_position(math::Vector2f const& pos);
        inline void set_velocity(math::Vector2f const& vel);
        inline void set_max_angular_speed(float const max_ang_spd);
        inline void set_rotation(float const rotation);
        inline void damp_angular_velocity(float const mod, std::chrono::milliseconds dt);

        inline math::Vector2f const& get_position() const;
        inline float                 get_rotation() const;
        inline float                 get_angular_velocity() const;
        inline math::Vector2f const& get_velocity() const;
        inline math::Vector2f        get_forward() const;
        inline float                 get_inverse_mass() const;
    };

} // namespace physics

#include "rigid_body.inl"

#endif // PHYSICS_RIGID_BODY_HPP
