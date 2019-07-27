#include "ai.hpp"
#include "attack_state.hpp"
#include "idle_state.hpp"
#include "caught_state.hpp"
#include "patrol_state.hpp"

ai::Ai::Ai( physics::RigidBody &rigid_body, entities::LevelTerrain &terrain)
           : m_attack(new AttackState())
           , m_caught(new CaughtState())
           , m_idle(new IdleState())
           , m_patrol(new PatrolState())
           , m_path_finder(terrain)
           , m_terrain(terrain)
           , m_state(m_patrol)
           , m_rigid_body(rigid_body)
           , m_path_finder_state(IDLE)
{
    m_path_finder.set_world_size(120);
    m_dimensions[X] = 6000;
    m_dimensions[Y] = 4000;
    m_goal_reached = true;
    srand(static_cast<unsigned int>(time(NULL)));
}
ai::Ai::~Ai()
{
    delete m_patrol;
    delete m_caught;
    delete m_idle;
    delete m_attack;
}
void ai::Ai::change_state(AiStates::states next_state)
{
    std::cout << "Change state " << m_state->get_state() << " -> " << next_state << std::endl;
    if (next_state == AiStates::ST_ATTACK)
        m_state = m_attack;
    else if (next_state == AiStates::ST_PATROL)
        m_state = m_patrol;
    else if (next_state == AiStates::ST_CAUGHT)
        m_state = m_caught;
    else if (next_state == AiStates::ST_IDLE)
        m_state = m_idle;
    m_state->execute(*this);
}
void ai::Ai::execute()
{
    m_state->execute(*this);
}
void ai::Ai::update(std::chrono::milliseconds const dt, float thrust, float yaw)
{
    m_current_position = m_rigid_body.get_position();
    update_path_finder(dt, thrust, yaw);
    execute();
}
void ai::Ai::update_path_finder(std::chrono::milliseconds const dt, float thrust, float yaw)
{
    m_deceleration = 0.2f;
    m_angular_damp = 3.0f;
    m_yaw = yaw;
    m_thrust = thrust;
    m_damp = 3.0f;
    math::Vector2f const rb_fwd = m_rigid_body.get_forward();
    math::Vector2f const rb_vel = m_rigid_body.get_velocity();
    switch (m_path_finder_state)
    {
    case IDLE:
        break;
    case SEARCHING:
    {
        //std::cout << "Pathfinder searching for shortest path " << ++m_search_cycles << std::endl;
        m_rigid_body.add_force(-m_rigid_body.get_velocity().normalized() * m_thrust * m_deceleration);
        if (m_path_finder.get_found_goal())
        {
            //std::cout << "Pathfinder found goal" << std::endl;
            //aaastd::cout << "Moving to location" << std::endl;
            m_path_finder_state = FOUND_GOAL;
            break;
        }
        ++m_search_cycles;
        if (m_search_cycles >= 59)
        {
            std::cout << "Skip search!" << std::endl;
            set_goal_reached(true);
            execute();
        }
        m_path_finder.find_path(m_current_position, m_target_position);
        break;
    }
    case FOUND_GOAL:
    {
        turn_direction = rand() % 2;
        m_distance_to_target = m_target_position - m_current_position;
        if (!check_if_at_goal())
        {
            m_offset = 0.3f;
            m_next_position = m_path_finder.get_next_position(m_current_position);
            if (m_next_position[X] == m_last_position[X] && m_next_position[Y] == m_last_position[Y])
                ++m_stuck_cycles;
            else
                m_stuck_cycles = 0;
            m_last_position = m_next_position;
            //std::cout << m_stuck_cycles << std::endl;
            if (m_stuck_cycles >= 200)
            {
                std::cout << "destroying terrain!" << std::endl;
                m_terrain.destroy_circle(math::Vector2i({ static_cast<int>(m_current_position[X])
                                                        , static_cast<int>(m_current_position[Y]) }), 150);
                m_stuck_cycles = 0;
            }
            m_distance_to_next_position = m_next_position - m_current_position;
            m_angle_to_node = atan2(m_next_position[Y] - m_current_position[Y]
                                  , m_next_position[X] - m_current_position[X]);
            m_plane_rotation = constrain_angle(m_rigid_body.get_rotation());
            m_plane_node_angle = m_angle_to_node - m_plane_rotation;
            m_angle_negative_offset = m_angle_to_node - m_offset;
            m_angle_positive_offset = m_angle_to_node + m_offset;
            m_velocity_angle = atan2(m_rigid_body.get_velocity()[Y], m_rigid_body.get_velocity()[X]);
            m_plane_velocity_angle = constrain_angle(m_velocity_angle - m_plane_rotation);

            //Too much sideways velocity
            if (m_plane_velocity_angle > 40.0f * M_PI / 180.0f
                && m_rigid_body.get_velocity().magnitude() > 40.0f)
            {
                //std::cout << "Right" << std::endl;
                m_rigid_body.add_torque(-1 * m_yaw);
                //m_rigid_body.add_force(-rb_vel.normalized() * THRUST);
                m_rigid_body.add_force(-m_rigid_body.get_velocity().normalized() * thrust * m_deceleration);
                //math::Vector2f::back()
            }
            if (m_plane_velocity_angle < -40.0f * M_PI / 180.0f
                && m_rigid_body.get_velocity().magnitude() > 40.0f)
            {
                //std::cout << "Left" << std::endl;
                m_rigid_body.add_torque(1 * m_yaw);
                //m_rigid_body.add_force(-rb_vel.normalized() * THRUST);
                m_rigid_body.add_force(-m_rigid_body.get_velocity().normalized() * thrust * m_deceleration);
                //math::Vector2f::forward()
            }

            //Break if angle towards target too big
            if (m_plane_node_angle > 50.0f * M_PI / 180.0f
                || m_plane_node_angle < -50.0f * M_PI / 180.0f)
            {
                //m_rigid_body.add_force(-rb_vel.normalized() * THRUST);
                m_rigid_body.add_force(-m_rigid_body.get_velocity().normalized() * thrust * m_deceleration);
            }

            //Turn if angle towards target too big
            if (m_plane_rotation <= m_angle_negative_offset
                || m_plane_rotation >= m_angle_positive_offset)
            {
                if (!get_rotate_direction(m_angle_to_node, m_plane_rotation))
                {
                    m_rigid_body.add_torque(1 * m_yaw);
                    //std::cout << "vasenpaan" << std::endl;
                }
                else
                {
                    m_rigid_body.add_torque(-1 * m_yaw);
                    //std::cout << "oikeaan" << std::endl;
                }
            }

            //Accelerate if angle towards target is small enough
            if (m_angle_negative_offset < m_plane_rotation
                && m_plane_rotation < m_angle_positive_offset)
            {
                //std::cout << "thrusting" << std::endl;
                //m_rigid_body.add_force(rb_fwd * THRUST);
                m_rigid_body.add_force(m_rigid_body.get_forward() * thrust);
            }
            m_rigid_body.damp_angular_velocity(m_damp, dt);
        }
        else
        {
            //std::cout << "Arrived at location" << std::endl << std::endl;
            set_goal_reached(true);
            m_path_finder_state = IDLE;
        }
        break;
        }
        default:
            break;
        }
}
int ai::Ai::get_path_finder_state()
{
    return m_path_finder_state;
}
void ai::Ai::patrol()
{
    if (get_goal_reached())
    {
        if (select_next_position() == true)
        {
            set_goal_reached(false);
            set_found_goal(false);
            set_path_finder_initialization(false);
            m_search_cycles = 0;
            set_path_finder_state(ai::Ai::SEARCHING);
        }
    }
}
void ai::Ai::chase()
{

}
void ai::Ai::flee()
{

}
void ai::Ai::caught()
{
    std::cout << "Caught target!" << std::endl;
}
void ai::Ai::set_target_position(math::Vector2f target_position)
{
    m_target_position = target_position;
}
bool ai::Ai::check_if_at_goal()
{
    if (m_distance_to_target.magnitude() < m_path_finder.get_radius())
        return true;
    return false;
}
void ai::Ai::set_path_finder_state(int path_finder_state)
{
    m_path_finder_state = path_finder_state;
}
void ai::Ai::set_found_goal(bool goal_found)
{
	m_path_finder.set_found_goal(goal_found);
}
void ai::Ai::set_goal_reached(bool reached)
{
    m_goal_reached = reached;
}
void ai::Ai::set_path_finder_initialization(bool init)
{
    m_path_finder.set_initialized_start_goal(init);
}
bool ai::Ai::get_path_finder_initialization()
{
    return m_path_finder.get_initialized_start_goal();
}
math::Vector2f ai::Ai::get_target_position()
{
    return m_target_position;
}
void ai::Ai::find_path_thread(math::Vector2f current_position, math::Vector2f target_position)
{
    m_path_finder.find_path(current_position, target_position);
}
bool ai::Ai::select_next_position()
{
    m_plane_rotation = constrain_angle(m_rigid_body.get_rotation());
    math::Vector2i current_pos;
    current_pos[X] = static_cast<int>(m_current_position[X]);
    current_pos[Y] = static_cast<int>(m_current_position[Y]);
    int x = current_pos[X] + 100;
    int y = current_pos[Y] + 100;
    if (m_plane_rotation <= 0 * M_PI / 180.0f && m_plane_rotation > -90.0f * M_PI / 180.0)
    {
        x = current_pos[X] + (rand() % m_random_point_scale + m_random_point_start);
        y = current_pos[Y] - (rand() % m_random_point_scale + m_random_point_start);
    }
    if (m_plane_rotation > 0 * M_PI / 180.0f && m_plane_rotation <= 90.0f * M_PI / 180.0)
    {
        x = current_pos[X] + (rand() % m_random_point_scale + m_random_point_start);
        y = current_pos[Y] + (rand() % m_random_point_scale + m_random_point_start);
    }
    if (m_plane_rotation >= 90.0f * M_PI / 180.0f && m_plane_rotation < 180.0f * M_PI / 180.0)
    {
        x = current_pos[X] - (rand() % m_random_point_scale + m_random_point_start);
        y = current_pos[Y] + (rand() % m_random_point_scale + m_random_point_start);
    }
    if (m_plane_rotation <= -90.0f * M_PI / 180.0f && m_plane_rotation > -179.0f * M_PI / 180.0)
    {
        x = current_pos[X] - (rand() % m_random_point_scale + m_random_point_start);
        y = current_pos[Y] - (rand() % m_random_point_scale + m_random_point_start);
    }
    if (x > 400
        && x < 5600
        && y > 400
        && y < 3600
        && !m_terrain.is_pixel_solid(math::Vector2i({ x, y }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x, y + 25 }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x, y + 50 }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x, y + 75 }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x, y + 100 }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x, y + 150 }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x + 25, y }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x + 50, y }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x + 75, y }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x + 100, y }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x + 150, y }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x, y - 25 }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x, y - 50 }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x, y - 75 }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x, y - 100 }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x, y - 150 }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x - 25, y }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x - 50, y }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x - 75, y }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x - 100, y }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x - 150, y }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x + 25, y + 25 }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x + 50, y + 50 }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x + 75, y + 75 }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x + 100, y + 100 }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x + 150, y + 150 }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x - 25, y - 25 }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x - 50, y - 50 }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x - 75, y - 75 }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x - 100, y - 100 }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x - 150, y - 150 }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x + 25, y - 25 }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x + 50, y - 50 }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x + 75, y - 75 }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x + 100, y - 100 }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x + 150, y - 150 }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x - 25, y + 25 }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x - 50, y + 50 }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x - 75, y + 75 }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x - 100, y + 100 }))
        && !m_terrain.is_pixel_solid(math::Vector2i({ x - 150, y + 150 }))
        )
    {
        m_target_position[X] = static_cast<float>(x);
        m_target_position[Y] = static_cast<float>(y);
        return true;
    }
    else
    {
        if (turn_direction)
            m_rigid_body.add_torque(1 * m_yaw);
        else
            m_rigid_body.add_torque(-1 * m_yaw);
        return false;
    }
    return false;
}
void ai::Ai::set_target_position(double x, double y)
{
    m_target_position[X] = static_cast<float>(x);
    m_target_position[Y] = static_cast<float>(y);
}
void ai::Ai::create_random_target_position()
{
    int x = rand() % m_dimensions[X];
    int y = rand() % m_dimensions[Y];
    math::Vector2f random_position;
    random_position[X] = static_cast<float>(x);
    random_position[Y] = static_cast<float>(y);
    float distance = sqrt((x - m_current_position[X]) * (x - m_current_position[X])
         + (y - m_current_position[Y]) * (y - m_current_position[Y]));
    while (distance > 500.0f)
    {
        x = rand() % m_dimensions[X];
        y = rand() % m_dimensions[Y];
        distance = sqrt((x - m_current_position[X]) * (x - m_current_position[X])
                              + (y - m_current_position[Y]) * (y - m_current_position[Y]));
        std::cout << "distance " << distance << std::endl;
    }
    m_target_position[X] = static_cast<float>(x);
    m_target_position[Y] = static_cast<float>(y);
    std::cout << "target" << std::endl;
    std::cout << m_target_position << std::endl << std::endl;
}
bool ai::Ai::is_pixel_solid(math::Vector2i const& coordinates)
{
    return m_terrain.is_pixel_solid(coordinates);
}
float ai::Ai::constrain_angle(float angle)
{
    return static_cast<float>(remainder(angle, 2.0*M_PI));
}
bool ai::Ai::get_rotate_direction(float target_angle, float rotation)
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
bool ai::Ai::get_goal_reached()
{
    return m_goal_reached;
}
