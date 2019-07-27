#include "controllable_plane.hpp"

#include <GLFW/glfw3.h>

#include "../input/keyboard.hpp"
#include "../input/mouse.hpp"
#include "../network/utilities/udp_packet.hpp"

namespace entities {

ControllablePlane::ControllablePlane(float const mass,
                                     float const max_spd, float const max_ang_spd,
                                     std::vector<physics::BoxCollider> const& collider,
                                     float const thrust, float const yaw,
                                     utilities::ResourceHolder<
                                         graphics::Texture, graphics::Textures
                                     >& tex_hld,
                                     logic::GameplayState* const g_st,
                                     graphics::Texture& tex,
                                     math::Vector2i const& src_pos, math::Vector2i const& src_dim,
                                     math::Vector2i const& dim,
                                     math::Vector2f const& pos)
    : Plane(mass, max_spd, max_ang_spd, collider, thrust, yaw, tex_hld, g_st, tex, src_pos, src_dim, dim, pos)
{ }

void ControllablePlane::handle_input(input::Keyboard const& kb, input::Mouse const& m,
                                     math::Vector2f const& m_world_pos,
                                     std::chrono::milliseconds const dt)
{
    static float const DECELERATION = 0.2f;
    static float const ANGULAR_DAMP = 3.0f;

    auto& curr_weap = m_weapons[m_current_weapon];
    math::Vector2f const turret_pos(curr_weap->get_turret_position());
    float const weap_rot = atan2f(m_world_pos[Y] - turret_pos[Y], m_world_pos[X] - turret_pos[X]);
    for (int i = 0; i < NUM_WEAPONS; ++i)
        m_weapons[i]->set_rotation(weap_rot);

    if (m.is_button_held(input::BUTTON_FIRE) && curr_weap->is_automatic())
        shoot();
    else if (m.was_button_pressed(input::BUTTON_FIRE))
        shoot();
    else if (kb.was_key_pressed(input::KEY_RELOAD))
        reload_weapon();

    if (kb.was_key_pressed(input::KEY_WEAPON_1))
        set_weapon(0);
    else if (kb.was_key_pressed(input::KEY_WEAPON_2))
        set_weapon(1);
    else if (kb.was_key_pressed(input::KEY_WEAPON_3))
        set_weapon(2);
    else if (kb.was_key_pressed(input::KEY_WEAPON_PREVIOUS))
        set_weapon(m_previous_weapon);
        

    if (kb.is_key_held(input::KEY_THRUST))
        m_rigid_body.add_force(m_rigid_body.get_forward() * m_thrust);
    if (kb.is_key_held(input::KEY_DECELERATE))
    {
        float const vel_mag = m_rigid_body.get_velocity().magnitude();
        if (vel_mag != 0.0f)
            m_rigid_body.add_force(
                -m_rigid_body.get_velocity().normalized() * m_thrust * DECELERATION
            );
    }

    float rot_dir = 0.0f;
    if (kb.is_key_held(input::KEY_YAW_LEFT))
        rot_dir += 1.0f;
    if (kb.is_key_held(input::KEY_YAW_RIGHT))
        rot_dir -= 1.0f;
    if (rot_dir != 0.0f)
        m_rigid_body.add_torque(rot_dir * m_yaw);
    else
        m_rigid_body.damp_angular_velocity(ANGULAR_DAMP, dt);
}

void ControllablePlane::process_packet(network::UdpPacket packet)
{
    if (packet.header_contains(network::UDP_H_POS))
    {
        char buffer[network::UDP_MAX_DATABLOCK_SIZE] = { '\0' };
        auto data = packet.get_data_block();
        float x, y, rot, velx, vely;

        // x
        auto byte = 0;
        for (auto i = 0; data[byte] != '|'; i++, byte++)
            buffer[i] = data[byte];
        x = static_cast<float>(atof(buffer));

        // y
        byte++;
        auto e = 0; while (buffer[e] != '\0') buffer[e++] = '\0';
        for (auto i = 0; data[byte] != '|'; i++, byte++)
            buffer[i] = data[byte];
        y = static_cast<float>(atof(buffer));

        // rot
        byte++;
        e = 0; while (buffer[e] != '\0') buffer[e++] = '\0';
        for (auto i = 0; data[byte] != '|'; i++, byte++)
            buffer[i] = data[byte];
        rot = static_cast<float>(atof(buffer));

        // vel x
        byte++;
        e = 0; while (buffer[e] != '\0') buffer[e++] = '\0';
        for (auto i = 0; data[byte] != '|'; i++, byte++)
            buffer[i] = data[byte];
        velx = static_cast<float>(atof(buffer));

        // vel y
        byte++;
        e = 0; while (buffer[e] != '\0') buffer[e++] = '\0';
        for (auto i = 0; data[byte] != '\0'; i++, byte++)
            buffer[i] = data[byte];
        vely = static_cast<float>(atof(buffer));

        m_rigid_body.set_position(math::Vector2f({ x, y }));
        m_rigid_body.set_rotation(rot);
        m_rigid_body.set_velocity(math::Vector2f({ velx, vely }));
    }
}

} // namespace entities
