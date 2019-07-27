#include "search_node.hpp"

float ai::SearchNode::calculate_heuristic_distance(SearchNode* node_end)
{
    //Manhattan
    //float x = static_cast<float>(fabs(static_cast<float>(this->m_x - node_end->m_x)));
    //float y = static_cast<float>(fabs(static_cast<float>(this->m_y - node_end->m_y)));
    //return x + y;

    //Chebyshev
    //float x = static_cast<float>(fabs(static_cast<float>(this->m_x - node_end->m_x)));
    //float y = static_cast<float>(fabs(static_cast<float>(this->m_y - node_end->m_y)));
    //return std::max(x, y);

    //Euclidean
    float this_x = static_cast<float>(this->m_x);
    float end_x = static_cast<float>(node_end->m_x);
    float this_y = static_cast<float>(this->m_y);
    float end_y = static_cast<float>(node_end->m_y);
    return static_cast<float>(sqrt((this_x - end_x) * (this_x - end_x) 
              + (this_y - end_y) * (this_y - end_y)));
}

int ai::SearchNode::get_x()
{
    return m_x;
}

int ai::SearchNode::get_y()
{
    return m_y;
}

int ai::SearchNode::get_id()
{
    return m_id;
}

float ai::SearchNode::get_accumulated_distance()
{
    return m_accumulated_distance;
}

float ai::SearchNode::get_heuristic_distance()
{
    return m_heuristic_distance;
}

int ai::SearchNode::get_world_size()
{
    return m_world_size;
}

float ai::SearchNode::get_f_score()
{
    return m_accumulated_distance + m_heuristic_distance;
}

void ai::SearchNode::set_x(int x)
{
    m_x = x;
}

void ai::SearchNode::set_y(int y)
{
    m_y = y;
}

void ai::SearchNode::set_world_size(int world_size)
{
    m_world_size = world_size;
}
void ai::SearchNode::set_accumulated_distance(float accumulated_distance)
{
    m_accumulated_distance = accumulated_distance;
}

void ai::SearchNode::set_heuristic_distance(float heuristic_distance)
{
    m_heuristic_distance = heuristic_distance;
}
