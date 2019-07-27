#include "uncontrollable_plane.hpp"

#include <GLFW/glfw3.h>

#include "../input/keyboard.hpp"
#include "../input/mouse.hpp"
#include "../network/utilities/udp_packet.hpp"

namespace entities {

UncontrollablePlane::UncontrollablePlane(float const mass,
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
    , m_keyheld_up(false)
    , m_keyheld_down(false)
    , m_keyheld_left(false)
    , m_keyheld_right(false)
    , m_keyheld_shoot(false)
    , m_keypressed_shoot(false)
    , m_keypressed_reload(false)
    , m_keypressed_switch_1(false)
    , m_keypressed_switch_2(false)
    , m_keypressed_switch_3(false)
    , m_keypressed_switch_q(false)
{ }

math::Vector2f UncontrollablePlane::get_position() const
{
    return m_rigid_body.get_position();
}

math::Vector2f UncontrollablePlane::get_velocity() const
{
    return m_rigid_body.get_velocity();
}

float UncontrollablePlane::get_rotation() const
{
    return m_rigid_body.get_rotation();
}

void UncontrollablePlane::handle_input(math::Vector2f const& m_world_pos,
                                       std::chrono::milliseconds const dt)
{
    static float const DECELERATION = 0.2f;
    static float const ANGULAR_DAMP = 3.0f;

    auto& curr_weap = m_weapons[m_current_weapon];
    math::Vector2f const turret_pos(curr_weap->get_turret_position());
    float const weap_rot = atan2f(m_world_pos[Y] - turret_pos[Y], m_world_pos[X] - turret_pos[X]);
    for (int i = 0; i < NUM_WEAPONS; ++i)
        m_weapons[i]->set_rotation(weap_rot);

    if (m_keyheld_shoot && curr_weap->is_automatic())
        shoot();
    else if (m_keypressed_shoot)
        shoot();
    else if (m_keypressed_reload)
        reload_weapon();

    if (m_keypressed_switch_1)
        set_weapon(0);
    else if (m_keypressed_switch_2)
        set_weapon(1);
    else if (m_keypressed_switch_3)
        set_weapon(2);
    else if (m_keypressed_switch_q)
        set_weapon(m_previous_weapon);


    if (m_keyheld_up)
        m_rigid_body.add_force(m_rigid_body.get_forward() * m_thrust);
    if (m_keyheld_down)
    {
        float const vel_mag = m_rigid_body.get_velocity().magnitude();
        if (vel_mag != 0.0f)
            m_rigid_body.add_force(
                -m_rigid_body.get_velocity().normalized() * m_thrust * DECELERATION
            );
    }

    float rot_dir = 0.0f;
    if (m_keyheld_left)
        rot_dir += 1.0f;
    if (m_keyheld_right)
        rot_dir -= 1.0f;
    if (rot_dir != 0.0f)
        m_rigid_body.add_torque(rot_dir * m_yaw);
    else
        m_rigid_body.damp_angular_velocity(ANGULAR_DAMP, dt);
}

void UncontrollablePlane::process_packet(network::UdpPacket packet)
{
    if (packet.header_contains(network::UDP_H_INPUT))
    {
        if (packet.move_contains(network::UDP_IN_UP))
            m_keyheld_up = packet.move_contains(network::UDP_IN_PRESS);
        if (packet.move_contains(network::UDP_IN_DOWN))
            m_keyheld_down = packet.move_contains(network::UDP_IN_PRESS);
        if (packet.move_contains(network::UDP_IN_LEFT))
            m_keyheld_left = packet.move_contains(network::UDP_IN_PRESS);
        if (packet.move_contains(network::UDP_IN_RIGHT))
            m_keyheld_right = packet.move_contains(network::UDP_IN_PRESS);
        if (packet.action_contains(network::UDP_IN_SHOOT))
        {
            m_keyheld_shoot    = packet.action_contains(network::UDP_IN_PRESS);
            m_keypressed_shoot = packet.action_contains(network::UDP_IN_PRESS);
        }
        if (packet.action_contains(network::UDP_IN_RELOAD))
            m_keypressed_reload = packet.action_contains(network::UDP_IN_PRESS);
        if (packet.action_contains(network::UDP_IN_SWITCH_WPN_1))
            m_keypressed_switch_1 = packet.action_contains(network::UDP_IN_PRESS);
        if (packet.action_contains(network::UDP_IN_SWITCH_WPN_2))
            m_keypressed_switch_2 = packet.action_contains(network::UDP_IN_PRESS);
        if (packet.action_contains(network::UDP_IN_SWITCH_WPN_3))
            m_keypressed_switch_3 = packet.action_contains(network::UDP_IN_PRESS);
        if (packet.action_contains(network::UDP_IN_SWITCH_WPN_Q))
            m_keypressed_switch_q = packet.action_contains(network::UDP_IN_PRESS);
        return;
    }
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

        //// weapon num
        //byte++;
        //auto weapon = static_cast<int>(data[byte]) % 2;

        //// weapon ammo
        //byte++;
        //auto ammo = static_cast<int>(data[byte]);

        m_rigid_body.set_position(math::Vector2f({ x, y }));
        m_rigid_body.set_rotation(rot);
        m_rigid_body.set_velocity(math::Vector2f({ velx, vely }));

        //current_weapon = weapon;
        //m_weapons[current_weapon]->set_rounds(ammo);
    }
}

void UncontrollablePlane::reset_frame()
{
    m_keypressed_shoot    = false;
    m_keypressed_reload   = false;
    m_keypressed_switch_1 = false;
    m_keypressed_switch_2 = false;
    m_keypressed_switch_3 = false;
    m_keypressed_switch_q = false;
}

} // namespace entities
