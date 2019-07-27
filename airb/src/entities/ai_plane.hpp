#ifndef ENTITIES_AI_PLANE_HPP
#define ENTITIES_AI_PLANE_HPP

#include <chrono>

#include "plane.hpp"
#include "../ai/ai.hpp"
//#include "../graphics/window.hpp"
//#include "../entities/level_terrain.hpp"
//#include "../input/keyboard.hpp"
//#include "../input/mouse.hpp"

namespace input
{

    class Keyboard;
    class Mouse;

}

namespace entities
{

    class AiPlane final : public Plane
    {
    private:
        ai::Ai m_ai;
        entities::LevelTerrain &m_terrain;
        clock_t m_weapon_switch_counter;
        clock_t m_shoot_counter;
    public:
        AiPlane(float const mass,
                float const max_spd, float const max_ang_spd,
                std::vector<physics::BoxCollider> const& collider,
                float const thrust, float const yaw,
                utilities::ResourceHolder<graphics::Texture, graphics::Textures>& tex_hld,
                logic::GameplayState* const g_st,
                graphics::Texture& tex,
                math::Vector2i const& src_pos, math::Vector2i const& src_dim,
                math::Vector2i const& dim,
                math::Vector2f const& pos,
                entities::LevelTerrain &terrain);
        //AiPlane(float const mass,
        //                  float const max_spd, float const max_ang_spd,
        //                  float const thrust, float const yaw,
        //                  graphics::Texture& tex,
        //                  math::Vector2i const& src_pos, math::Vector2i const& src_dim,
        //                  math::Vector2i const& dim,
        //                  math::Vector2f const& pos,
        //                  entities::LevelTerrain &terrain);
        ~AiPlane() = default;

        AiPlane(AiPlane const&) = delete;
        AiPlane& operator= (AiPlane const&) = delete;

        //void handle_input(math::Vector2f cursor_pos, input::Keyboard const& kb, input::Mouse const& m,
        //                  math::Vector2f const& m_world_pos, std::chrono::milliseconds const dt);
        void handle_input(math::Vector2f cursor_pos, input::Keyboard const& kb, input::Mouse const& m,
                          math::Vector2f const& m_world_pos, std::chrono::milliseconds const dt);
        void update_behavior(math::Vector2f enemy_position, std::chrono::milliseconds const dt);
        bool get_rotate_direction(float target_angle, float rotation);
        float constrain_angle(float angle);
    };

} // namespace entities

#endif // ENTITIES_AI_PLANE_HPP