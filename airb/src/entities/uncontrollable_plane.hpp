#ifndef ENTITIES_UNCONTROLLABLE_PLANE_HPP
#define ENTITIES_UNCONTROLLABLE_PLANE_HPP

#include <chrono>

#include "plane.hpp"

namespace input {

class Keyboard;
class Mouse;

} // namespace input

namespace network {

class UdpPacket;

} // namespace network

namespace entities {

class UncontrollablePlane final : public Plane
{
private:
    bool m_keyheld_up;
    bool m_keyheld_down;
    bool m_keyheld_left;
    bool m_keyheld_right;
    bool m_keyheld_shoot;
    bool m_keypressed_shoot;
    bool m_keypressed_reload;
    bool m_keypressed_switch_1;
    bool m_keypressed_switch_2;
    bool m_keypressed_switch_3;
    bool m_keypressed_switch_q;

public:
    UncontrollablePlane(float const mass,
                        float const max_spd, float const max_ang_spd,
                        std::vector<physics::BoxCollider> const& collider,
                        float const thrust, float const yaw,
                        utilities::ResourceHolder<graphics::Texture, graphics::Textures>& tex_hld,
                        logic::GameplayState* const g_st,
                        graphics::Texture& tex,
                        math::Vector2i const& src_pos, math::Vector2i const& src_dim,
                        math::Vector2i const& dim,
                        math::Vector2f const& pos);
    ~UncontrollablePlane() = default;

    math::Vector2f get_position() const;
    math::Vector2f get_velocity() const;
    float          get_rotation() const;

    void handle_input(math::Vector2f const& m_world_pos,
                      std::chrono::milliseconds const dt);
    void process_packet(network::UdpPacket packet);

    void reset_frame();

};

} // namespace entities

#endif // ENTITIES_UNCONTROLLABLE_PLANE_HPP
