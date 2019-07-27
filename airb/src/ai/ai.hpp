#pragma once
#ifndef AI_AI_HPP
#define AI_AI_HPP

#define _USE_MATH_DEFINES

#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <math.h>
#include <thread>

#include "../entities/level_terrain.hpp"
#include "../math/matrix.hpp"
#include "../physics/rigid_body.hpp"
#include "ai_states.hpp"
#include "attack_state.hpp"
#include "caught_state.hpp"
#include "idle_state.hpp"
#include "path_finding.hpp"
#include "patrol_state.hpp"

class AttackState;
class CaughtState;
class IdleState;
class PatrolState;

namespace ai
{

class Ai
{
public:
    enum PathFinderStates
    {
        IDLE,
        SEARCHING,
        FOUND_GOAL
    };
private:
    float m_deceleration;
    float m_angular_damp;
    float m_yaw;
    float m_thrust;
    float m_damp;
    int m_random_point_scale = 100;
    int m_random_point_start = 200;
    int turn_direction = 0;
    int m_stuck_cycles = 0;
    AttackState* m_attack;
    CaughtState* m_caught;
    IdleState* m_idle;
    PatrolState* m_patrol;
    AiStates* m_state;
    PathFinding m_path_finder;
    physics::RigidBody &m_rigid_body;
    entities::LevelTerrain &m_terrain;
    int m_path_finder_state;
    int m_search_cycles = 0;
    float m_angle_to_node;
    float m_plane_rotation;
    float m_angle_negative_offset;
    float m_angle_positive_offset;
	float m_plane_node_angle;
    float m_offset;
    float m_velocity_angle;
    float m_plane_velocity_angle;
    bool m_goal_reached;
    math::Vector2i m_dimensions;
    math::Vector2f m_current_position;
    math::Vector2f m_target_position;
    math::Vector2f m_distance_to_target;
    math::Vector2f m_next_position;
    math::Vector2f m_distance_to_next_position;
    math::Vector2i m_next_patrol_target;
    math::Vector2f m_last_position = math::Vector2f({ -1.0, -1.0 });
    
public:
    Ai(physics::RigidBody &rigid_body, entities::LevelTerrain &terrain);
    virtual ~Ai();
    virtual void execute();
    virtual void change_state(AiStates::states next_state);
    void update(std::chrono::milliseconds const dt, float thrust, float yaw);
    void update_path_finder(std::chrono::milliseconds const dt, float thrust, float yaw);
    void patrol();
    void chase();
    void flee();
    void caught();
    void set_target_position(math::Vector2f target_position);
    void set_path_finder_initialization(bool init);
    void set_path_finder_state(int path_finder_state);
	void set_found_goal(bool goal_found);
    void set_goal_reached(bool reached);
    void set_target_position(double x, double y);
    void create_random_target_position();
    bool is_pixel_solid(math::Vector2i const& coordinates);
    bool check_if_at_goal();
    bool get_path_finder_initialization();
    bool get_rotate_direction(float target_angle, float rotation);
    bool get_goal_reached();
    int get_path_finder_state();
    float constrain_angle(float angle);
    math::Vector2f get_target_position();
    void find_path_thread(math::Vector2f m_current_position, math::Vector2f m_target_position);
    bool select_next_position();
};
} // namespace ai
#endif // AI_AI_HPP
