#ifndef NETWORK_PLAYER_HPP
#define NETWORK_PLAYER_HPP

#include <thread>
#include <WinSock2.h>

#include "connection.hpp"
#include "utilities/config.hpp"
#include "../entities/uncontrollable_plane.hpp"

namespace network {

class Player final : public Connection
{
private:
    math::Vector2f  m_cursor_pos;
    std::unique_ptr<entities::UncontrollablePlane> m_plane;

public:
    explicit Player(sockaddr_in const si);
    explicit Player(Connection parent);
             Player(U8 const team, U8 const pos);
            ~Player() = default;

    void process_packet     (UdpPacket packet) const;
    void set_cursor_position(float x, float y);
    void set_plane          (std::unique_ptr<entities::UncontrollablePlane> plane);
    void set_weapon_ammo    (char ammo);
    void set_weapon_num     (char num);
    void update             (std::chrono::milliseconds dt, entities::LevelTerrain& terr) const;

    entities::UncontrollablePlane& get_plane() const;
    math::Vector2f      get_cursor_position     () const;
    math::Vector2f      get_position            () const;
    math::Vector2f      get_velocity            () const;
    float               get_rotation            () const;
    graphics::Sprite&   get_sprite              () const;
    char                get_weapon_ammo         () const;
    char                get_weapon_num          () const;
    graphics::Sprite&   get_weapon_sprite       () const;
    physics::RigidBodyWithCollider& get_rigid_body() const;
};

} // namespace network

#endif // NETWORK_PLAYER_HPP
