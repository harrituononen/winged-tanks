#ifndef ENTITIES_CONTROLLABLE_PLANE_HPP
#define ENTITIES_CONTROLLABLE_PLANE_HPP

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

class ControllablePlane final : public Plane
{
public:
     ControllablePlane(float const mass,
                       float const max_spd, float const max_ang_spd,
                       std::vector<physics::BoxCollider> const& collider,
                       float const thrust, float const yaw,
                       utilities::ResourceHolder<graphics::Texture, graphics::Textures>& tex_hld,
                       logic::GameplayState* const g_st,
                       graphics::Texture& tex,
                       math::Vector2i const& src_pos, math::Vector2i const& src_dim,
                       math::Vector2i const& dim,
                       math::Vector2f const& pos);
    ~ControllablePlane() = default;

    ControllablePlane            (ControllablePlane const&) = delete;
    ControllablePlane& operator= (ControllablePlane const&) = delete;

    void handle_input(input::Keyboard const& kb, input::Mouse const& m,
                      math::Vector2f const& m_world_pos,
                      std::chrono::milliseconds const dt);

    void process_packet(network::UdpPacket packet);
};

} // namespace entities

#endif // ENTITIES_CONTROLLABLE_PLANE_HPP
