#include "player.hpp"

#pragma warning(disable : 4996)

#include <ctime>

#include "../network/utilities/udp_packet.hpp"

namespace network {

Player::Player(sockaddr_in const si)
    : Connection(si)
    , m_plane(nullptr)
{ }

Player::Player(Connection parent)
    : Connection(parent)
    , m_plane(nullptr)
{ }

Player::Player(U8 const t, U8 const pos)
    : Connection(t, pos)
{ }

void Player::process_packet(UdpPacket packet) const
{
    if (packet.header_contains(UDP_H_INPUT) ||
        packet.header_contains(UDP_H_POS))
    {
        m_plane->process_packet(packet);
    }
}

void Player::set_cursor_position(float x, float y)
{
    m_cursor_pos = math::Vector2f({x, y});
    auto const turret_pos = m_plane->get_weapon().get_turret_position();
    m_plane->get_weapon().set_rotation(atan2f(m_cursor_pos[Y] - turret_pos[Y],
                                              m_cursor_pos[X] - turret_pos[X]));
}

void Player::set_plane(std::unique_ptr<entities::UncontrollablePlane> plane)
{
    m_plane = std::move(plane);
}

entities::UncontrollablePlane& Player::get_plane() const
{
    return *m_plane;
}
void Player::update(std::chrono::milliseconds dt, entities::LevelTerrain& terr) const
{
    m_plane->update(dt, terr);
    m_plane->handle_input(m_cursor_pos, dt);
    m_plane->reset_frame();
}

math::Vector2f Player::get_cursor_position() const
{
    return m_cursor_pos;
}

math::Vector2f Player::get_position() const
{
    return m_plane->get_position();
}

math::Vector2f Player::get_velocity() const
{
    return m_plane->get_velocity();
}

float Player::get_rotation() const
{
    return m_plane->get_rotation();
}

graphics::Sprite& Player::get_sprite() const
{
    return m_plane->get_sprite();
}

char Player::get_weapon_ammo() const
{
    return '0' + m_plane->get_ammo();
}

char Player::get_weapon_num() const
{
    return '0' + m_plane->get_current_weapon();
}

graphics::Sprite& Player::get_weapon_sprite() const
{
    return m_plane->get_weapon_sprite();
}

physics::RigidBodyWithCollider& Player::get_rigid_body() const
{
    return m_plane->get_rigid_body();
}

}
