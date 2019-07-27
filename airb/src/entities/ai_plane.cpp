#include "ai_plane.hpp"

#include <GLFW/glfw3.h>

#include "../input/keyboard.hpp"
#include "../input/mouse.hpp"


namespace entities
{
    AiPlane::AiPlane(float const mass,
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
                                         math::Vector2f const& pos,
                                         entities::LevelTerrain &terrain)
        : Plane(mass, max_spd, max_ang_spd, collider, thrust, yaw, tex_hld, g_st, tex, src_pos, src_dim, dim, pos)
        , m_terrain(terrain), m_ai(m_rigid_body, terrain)
    {
        srand(static_cast<unsigned int>(time(NULL)));
        m_weapon_switch_counter = clock();
        m_shoot_counter = clock();
        m_ai.change_state(ai::AiStates::ST_PATROL);
    }

    void AiPlane::handle_input(math::Vector2f cursor_pos, input::Keyboard const& kb, input::Mouse const& m,
                                         math::Vector2f const& m_world_pos,
                                         std::chrono::milliseconds const dt)
    {
        static float const DECELERATION = 0.2f;
        static float const ANGULAR_DAMP = 3.0f;

            if (m.was_button_pressed(GLFW_MOUSE_BUTTON_2))
            {
                //std::cout << cursor_pos << std::endl;
                m_ai.set_goal_reached(false);
                m_ai.set_found_goal(false);
                m_ai.set_path_finder_initialization(false);
                m_ai.set_target_position(m_world_pos[X], m_world_pos[Y]);
                m_ai.set_path_finder_state(ai::Ai::SEARCHING);
                m_ai.change_state(ai::AiStates::ST_PATROL);
            }
    }

    void AiPlane::update_behavior(math::Vector2f enemy_position, std::chrono::milliseconds const dt)
    {
        static float const DECELERATION = 0.2f;
        static float const ANGULAR_DAMP = 3.0f;

        if (clock() - m_weapon_switch_counter > float(rand() % 10000 + 10000) / CLOCKS_PER_SEC * 1000)
        {
            set_weapon(rand() % 3);
            m_weapon_switch_counter = clock();
        }

        auto& curr_weap = m_weapons[m_current_weapon];
        float rot_dir = 0;
        math::Vector2f const turret_pos(curr_weap->get_turret_position());
        float turret_rot(curr_weap->get_rotation());
        float turret_rot_to_target = atan2f(enemy_position[Y] - turret_pos[Y],
                                            enemy_position[X] - turret_pos[X]);
        if ((enemy_position - turret_pos).magnitude() < 700.0f)
        {
            if (turret_rot > turret_rot_to_target + 0.15f || turret_rot < turret_rot_to_target - 0.15f)
            {
                if (get_rotate_direction(turret_rot_to_target, turret_rot))
                    rot_dir = -0.05f;
                else
                    rot_dir = 0.05f;
                curr_weap->set_rotation(constrain_angle(curr_weap->get_rotation() + rot_dir));
                //curr_weap->set_rotation(atan2f(enemy_position[Y] - turret_pos[Y],
                //                               enemy_position[X] - turret_pos[X]));
            }
            else
            {
                if (clock() - m_shoot_counter > float(rand() % 2500) / CLOCKS_PER_SEC * 1000)
                {
                    shoot();
                    m_shoot_counter = clock();
                }
            }
        }
        m_ai.update(dt, m_thrust, m_yaw);
    }

    bool AiPlane::get_rotate_direction(float target_angle, float rotation)
    {
        float result_angle = target_angle - rotation;
        if (result_angle < -M_PI)
            result_angle += static_cast<float>(2.0f*M_PI);
        if (result_angle > M_PI)
            result_angle -= static_cast<float>(2.0f*M_PI);
        if (result_angle > 0)
            return false;
        return true;
    }
    float AiPlane::constrain_angle(float angle)
    {
        return static_cast<float>(remainder(angle, 2.0*M_PI));
    }
} // namespace entities
