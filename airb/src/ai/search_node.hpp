#pragma once
#ifndef AI_SEARCH_NODE_HPP
#define AI_SEARCH_NODE_HPP

#include <algorithm>
#include <iostream>
#include <math.h>

namespace ai
{

class SearchNode
{
public:
    SearchNode() : m_parent(nullptr) {}
    SearchNode(int x, int y, int world_size, SearchNode* parent = nullptr) 
        : m_x(x), m_y(y)
        , m_parent(parent), m_accumulated_distance(0)
        , m_heuristic_distance(0)
        , m_world_size(120)
        , m_already_visited(-1)
    {
        m_id = m_y * m_world_size + m_x; m_visited = false;
    }

    int m_x;
    int m_y;
    int m_id;
    int m_world_size;
    int m_already_visited;
    bool m_visited;
    float m_accumulated_distance;
    float m_heuristic_distance;
    SearchNode* m_parent;
    int get_x();
    int get_y();
    int get_id();
    int get_world_size();
    float get_accumulated_distance();
    float get_heuristic_distance();
    float get_f_score();
    float calculate_heuristic_distance(SearchNode* node_end);
    void set_x(int x);
    void set_y(int y);
    void set_world_size(int world_size);
    void set_accumulated_distance(float accumulated_distance);
    void set_heuristic_distance(float heuristic_distance);
};
} // namespace ai
#endif // AI_SEARCH_NODE_HPP
