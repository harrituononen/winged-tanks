#include "path_finding.hpp"


ai::PathFinding::PathFinding(entities::LevelTerrain &terrain)
    : m_initialized_start_goal(false)
    , m_found_goal(false)
    , m_path_number(0)
    , m_terrain(terrain)
{
    m_world_size = 120;
    for (int i = 0; i < 80; i = ++i)
    {
        for (int j = 0; j < 120; ++j)
        {
            m_nodes.push_back(new SearchNode(j , i, m_world_size, nullptr));
        }

    }
}

ai::PathFinding::~PathFinding()
{
    for (unsigned int i = 0; i < m_nodes.size(); ++i)
    {
        delete m_nodes[i];
    }
}

float ai::PathFinding::get_radius()
{
    return radius;
}

void ai::PathFinding::find_path(math::Vector2f current_position, math::Vector2f target_position)
{
    if (!m_initialized_start_goal)
    {
        for (unsigned int i = 0; i < m_nodes.size(); ++i)
            m_nodes[i]->m_visited = false;

        m_open_list.clear();
        m_path_to_goal.clear();
        
        //for (int i = 0; i < m_visited_list.size(); ++i)
        //    delete m_visited_list[i];
        //m_visited_list.clear();

        //Initialize start
        m_start_node = m_nodes[ (static_cast<int>(current_position[Y]) / node_size_int)
                              * m_world_size
                              + (static_cast<int>(current_position[X]) / node_size_int) ];
        
        //Initialize goal
        m_goal_node = m_nodes[ (static_cast<int>(target_position[Y]) / node_size_int)
                             * m_world_size
                             + (static_cast<int>(target_position[X]) / node_size_int) ];
        
		m_found_goal = false;
        set_start_and_goal();
        m_initialized_start_goal = true;
    }
    if(m_initialized_start_goal)
        continue_path();
}

void ai::PathFinding::set_start_and_goal()
{
    m_goal_node->m_parent = m_goal_node;
    m_start_node->m_accumulated_distance = 0;
    m_start_node->m_heuristic_distance = m_start_node->calculate_heuristic_distance(m_goal_node);
    m_start_node->m_parent = nullptr;

    m_open_list.push_back(m_start_node);
    make_heap(m_open_list.begin(), m_open_list.end(), CompareFCost());
}

void ai::PathFinding::set_world_size(int world_size)
{
    m_world_size = world_size;
}

ai::SearchNode* ai::PathFinding::get_next_node()
{
    SearchNode* next_node = nullptr;
    //std::cout << ++cycle_counter << std::endl;
    if (!m_open_list.empty())
    {
        next_node = m_open_list.front();
        //m_visited_list.push_back(next_node);
        next_node->m_visited = true;
        m_open_list.erase(m_open_list.begin());
        push_heap(m_open_list.begin(), m_open_list.end(), CompareFCost());
    }

    return next_node;
}

void ai::PathFinding::find_neighbours(int x, int y)
{
    m_neighbours.clear();
    m_neighbours.push_back(m_nodes[(y + 1) * m_world_size + (x - 1)]);
    m_neighbours.push_back(m_nodes[(y + 1) * m_world_size + x]);
    m_neighbours.push_back(m_nodes[(y + 1) * m_world_size + (x + 1)]);
    m_neighbours.push_back(m_nodes[y * m_world_size + (x - 1)]);
    m_neighbours.push_back(m_nodes[y * m_world_size + (x + 1)]);
    m_neighbours.push_back(m_nodes[(y - 1) * m_world_size + (x - 1)]);
    m_neighbours.push_back(m_nodes[(y - 1) * m_world_size + x]);
    m_neighbours.push_back(m_nodes[(y - 1) * m_world_size + (x + 1)]);
}
void ai::PathFinding::identify_successors(SearchNode * current_node, SearchNode * start_node, SearchNode * goal_node)
{
    find_neighbours(current_node->m_x, current_node->m_y);
    m_successors.clear();
    for (unsigned int i = 0; i < m_neighbours.size(); ++i)
    {
        SearchNode* temp_node = m_neighbours[i];

        //Direction to neighbour
        int d_x = temp_node->m_x - current_node->m_x;
        int d_y = temp_node->m_y - current_node->m_y;

        //Try to find a node to jump to
        SearchNode* jump_point = jump(current_node->m_x, current_node->m_y, d_x, d_y, start_node, goal_node);

        if (jump_point != nullptr)
            m_successors.push_back(jump_point);
    }
}

ai::SearchNode* ai::PathFinding::jump(int c_x, int c_y, int d_x, int d_y, SearchNode * start, SearchNode * goal)
{
    // cX, cY - Current Node Position,  dX, dY - Direction
    // Position of new node we are going to consider:
    int next_x = c_x + d_x;
    int next_y = c_y + d_y;

    if (is_node_blocked(next_x, next_y))
        return nullptr;

    // If the node is the goal return it
    if (next_x == goal->m_x && next_y == goal->m_y)
        return m_nodes[next_y * m_world_size + next_x];

    // Diagonal Case   
    if (d_x != 0 && d_y != 0)
    {
        if ((!is_node_blocked(next_x - d_x, next_y + d_y) && is_node_blocked(next_x - d_x, next_y)) ||
            (!is_node_blocked(next_x + d_x, next_y - d_y) && is_node_blocked(next_x, next_y - d_y)))
        {
            return m_nodes[next_y * m_world_size + next_x];
        }
        // Check in horizontal and vertical directions for forced neighbors
        // This is a special case for diagonal direction
        if (jump(next_x, next_y, d_x, c_y, start, goal) != nullptr ||
            jump(next_x, next_y, c_x, d_y, start, goal) != nullptr)
        {
            return m_nodes[next_y * m_world_size + next_x];
        }
    }
    else
    {
        // Horizontal case
        if (d_x != 0)
        {
            if ((!is_node_blocked(next_x + d_x, next_y + 1) && is_node_blocked(next_x, next_y + 1)) ||
                (!is_node_blocked(next_x + d_x, next_y - 1) && is_node_blocked(next_x, next_y - 1)))
            {

                return m_nodes[next_y * m_world_size + next_x];
            }
        }
        else /// Vertical case
        {
            if ((!is_node_blocked(next_x + 1, next_y + d_y) && is_node_blocked(next_x + 1, next_y)) ||
                (!is_node_blocked(next_x - 1, next_y + d_y) && is_node_blocked(next_x - 1, next_y)))
            {
                return m_nodes[next_y * m_world_size + next_x];

            }
        }
    }
    /// If forced neighbor was not found try next jump point
    return jump(next_x, next_y, d_x, d_y, start, goal);
    
}

void ai::PathFinding::is_path_opened(int x, int y, float new_cost, SearchNode* parent)
{
    if (is_node_blocked(x, y))
        return;
    if (x > 1 && y > 1 && x < 119 && y < 79)
        if (is_node_blocked(x + 1, y - 1)
            || is_node_blocked(x - 1, y + 1)
            || is_node_blocked(x, y + 1)
            || is_node_blocked(x + 1, y + 1)
            || is_node_blocked(x - 1, y)
            || is_node_blocked(x + 1, y)
            || is_node_blocked(x - 1, y - 1)
            || is_node_blocked(x, y - 1))
            return;

    int id = y * m_world_size + x;

    //for (int i = 0; i < m_visited_list.size(); ++i)
    //    if(id == m_visited_list[i]->m_id)
    //        return;

    if (m_nodes[id]->m_visited == true)
        return;
    
    //SearchNode* new_child = new SearchNode(x, y, m_world_size, parent);
    SearchNode* new_child = m_nodes[y * m_world_size + x];
    SearchNode* last_parent = new_child->m_parent;
    float last_accumulated = new_child->m_accumulated_distance;
    float last_heuristic = new_child->m_heuristic_distance;
    new_child->m_parent = parent;
    new_child->m_accumulated_distance = new_cost;
    new_child->m_heuristic_distance = parent->calculate_heuristic_distance(m_goal_node);

    for (unsigned int i = 0; i < m_open_list.size(); ++i)
    {
        if(id == m_open_list[i]->m_id)
        {
            float new_f = new_child->m_accumulated_distance 
                                     + new_cost
                                     + m_open_list[i]->m_heuristic_distance;

            if (m_open_list[i]->get_f_score() > new_f)
            {
                m_open_list[i]->m_accumulated_distance = new_child->m_accumulated_distance 
                                                                    + new_cost;
                m_open_list[i]->m_parent = new_child;

            }
            else
            {
                //delete new_child;
                new_child->m_parent = last_parent;
                new_child->m_accumulated_distance = last_accumulated;
                new_child->m_heuristic_distance = last_heuristic;
                return;
            }
        }
    }
    m_open_list.push_back(new_child);
    push_heap(m_open_list.begin(), m_open_list.end(), CompareFCost());
}

void ai::PathFinding::continue_path()
{
    for (int i = 0; i < search_cycles; ++i)
    {
        if (m_open_list.empty())
            return;

        SearchNode* current_node = get_next_node();
        //SearchNode* current_parent = current_node->m_parent;

        //identify_successors(current_node, m_start_node, m_goal_node);
        //if (m_successors.size() != 0)
        //{
        //    current_node = m_successors.back();
        //    current_node->m_parent = current_parent;
        //    current_node->m_visited = true;
        //}

        if (current_node->m_id == m_goal_node->m_id)
        {
            m_goal_node->m_parent = current_node->m_parent;
            SearchNode* get_path;
            math::Vector2f goal_coordinates;
            for (get_path = m_goal_node; get_path != nullptr; get_path = get_path->m_parent)
            {
                goal_coordinates[X] = get_path->m_x * node_size;
                goal_coordinates[Y] = get_path->m_y * node_size;
                m_path_to_goal.push_back(goal_coordinates);
            }
            m_found_goal = true;
        }
        else
        {
            // Left side
            is_path_opened(current_node->m_x - 1, current_node->m_y,
                           current_node->m_accumulated_distance + 1,
                           current_node);
            // Right side
            is_path_opened(current_node->m_x + 1, current_node->m_y,
                           current_node->m_accumulated_distance + 1,
                           current_node);
            // Up
            is_path_opened(current_node->m_x, current_node->m_y + 1,
                           current_node->m_accumulated_distance + 1,
                           current_node);
            // Down
            is_path_opened(current_node->m_x, current_node->m_y - 1,
                           current_node->m_accumulated_distance + 1,
                           current_node);
            // Left-up
            is_path_opened(current_node->m_x - 1, current_node->m_y + 1,
                           current_node->m_accumulated_distance + 1.4141f,
                           current_node);
            // Right-up
            is_path_opened(current_node->m_x + 1, current_node->m_y + 1,
                           current_node->m_accumulated_distance + 1.4141f,
                           current_node);
            // Left-down
            is_path_opened(current_node->m_x - 1, current_node->m_y - 1,
                           current_node->m_accumulated_distance + 1.4141f,
                           current_node);
            // Right-down
            is_path_opened(current_node->m_x + 1, current_node->m_y - 1,
                           current_node->m_accumulated_distance + 1.4141f,
                           current_node);

            for (unsigned int j = 0; j < m_open_list.size(); ++j)
            {
                if (current_node->m_id == m_open_list[j]->m_id)
                {
                    m_open_list.erase(m_open_list.begin() + j);
                }
            }
        }
    }
}


math::Vector2f ai::PathFinding::get_next_position(math::Vector2f current_position)
{
    unsigned int index = 1;
    math::Vector2f next_position;
    next_position[X] = m_path_to_goal[m_path_to_goal.size() - index][X] + (node_size / 2);
    next_position[Y] = m_path_to_goal[m_path_to_goal.size() - index][Y] + (node_size / 2);

    math::Vector2f distance = next_position - current_position;

    if(index < m_path_to_goal.size())
    {
        if(distance.magnitude() < radius)
        {
            m_path_to_goal.erase(m_path_to_goal.end() - index);
            //std::cout << "Reached node" << std::endl;
        }
    }
    return next_position;
}

bool ai::PathFinding::get_found_goal()
{
    return m_found_goal;
}

bool ai::PathFinding::get_initialized_start_goal()
{
    return m_initialized_start_goal;
}

bool ai::PathFinding::is_node_blocked(int x, int y)
{
    if (x * node_size_int > 6000
        || x * node_size_int < 0
        || y * node_size_int > 4000
        || y * node_size_int < 0)
        return true;
    node_center = math::Vector2i({ x * node_size_int + (node_size_int / 2)
                                 , y * node_size_int + (node_size_int / 2) });

    node_center_left = math::Vector2i({ x * node_size_int
                                      , y * node_size_int + (node_size_int / 2) });

    node_center_right = math::Vector2i({ x * node_size_int + node_size_int
                                       , y * node_size_int + (node_size_int / 2) });

    node_center_top = math::Vector2i({ x * node_size_int + (node_size_int / 2)
                                     , y * node_size_int + node_size_int });

    node_center_bottom = math::Vector2i({ x * node_size_int + (node_size_int / 2)
                                        , y * node_size_int });

    node_top_left = math::Vector2i({ x * node_size_int
                                   , y * node_size_int + node_size_int });

    node_top_right = math::Vector2i({ x * node_size_int + node_size_int
                                    , y * node_size_int + node_size_int });

    node_bottom_left = math::Vector2i({ x * node_size_int
                                      , y * node_size_int });

    node_bottom_right = math::Vector2i({ x * node_size_int + node_size_int
                                       , y * node_size_int });

    for (int i = 0; i < 50; ++i)
    {
        if (m_terrain.is_pixel_solid(math::Vector2i({ x * node_size_int + i
                                                    , y * node_size_int + (node_size_int / 2) })))
            return true;
    }

    //for (int i = 0; i < 70; ++i)
    //{
    //    if (m_terrain.is_pixel_solid(math::Vector2i({ x * node_size_int + i
    //                                                , y * node_size_int + node_size_int - i})))
    //        return true;
    //}

    if (m_terrain.is_pixel_solid(node_center)
        || m_terrain.is_pixel_solid(node_center_left)
        || m_terrain.is_pixel_solid(node_center_right)
        || m_terrain.is_pixel_solid(node_center_top)
        || m_terrain.is_pixel_solid(node_center_bottom)
        || m_terrain.is_pixel_solid(node_top_left)
        || m_terrain.is_pixel_solid(node_top_right)
        || m_terrain.is_pixel_solid(node_bottom_left)
        || m_terrain.is_pixel_solid(node_bottom_right))
        return true;
    return false;
}

void ai::PathFinding::set_initialized_start_goal(bool init)
{
    m_initialized_start_goal = init;
}

void ai::PathFinding::set_found_goal(bool goal_found)
{
	m_found_goal = goal_found;
}
