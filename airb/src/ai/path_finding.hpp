#pragma once
#ifndef AI_PATH_FINDER_HPP
#define AI_PATH_FINDER_HPP

#include <queue>
#include <vector>
#include <set>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iterator>

#include "../entities/level_terrain.hpp"
#include "../math/matrix.hpp"
#include "priority_queue.hpp"
#include "search_node.hpp"

namespace ai
{

struct CompareFCost
{
    bool operator()(SearchNode* const lhs, SearchNode* const rhs) const
    {
        // return "true" if "rhs" is ordered before "lhs"
        return lhs->get_f_score() > rhs->get_f_score();
    }
};

class PathFinding
{
private:
    float radius = 150.0f;
	float node_size = 50.0f;
    int cycle_counter = 0;
    int search_cycles = 10;
    entities::LevelTerrain &m_terrain;
    bool m_found_goal;
    bool m_initialized_start_goal;
    SearchNode* m_start_node;
    SearchNode* m_goal_node;
    int m_world_size;
    int node_size_int = static_cast<int>(node_size);
    int m_path_number;
    math::Vector2i node_center;
    math::Vector2i node_center_left;
    math::Vector2i node_center_right;
    math::Vector2i node_center_top;
    math::Vector2i node_center_bottom;
    math::Vector2i node_top_left;
    math::Vector2i node_top_right;
    math::Vector2i node_bottom_left;
    math::Vector2i node_bottom_right;
    std::vector<SearchNode*> m_open_list;
    std::vector<SearchNode*> m_visited_list;
    std::vector<SearchNode*> m_nodes;
    std::vector<SearchNode*> m_neighbours;
    std::vector<SearchNode*> m_successors;
    std::vector<math::Vector2f> m_path_to_goal;

public:
    PathFinding(entities::LevelTerrain &terrain);
    ~PathFinding();
    float get_radius();
    bool get_found_goal();
    bool get_initialized_start_goal();
    bool is_node_blocked(int x, int y);
    void set_initialized_start_goal(bool init);
	void set_found_goal(bool goal_found);
    void set_start_and_goal();
    void set_world_size(int world_size);
    void find_path(math::Vector2f current_position, math::Vector2f target_position);
    void is_path_opened(int x, int y, float new_cost, SearchNode* parent);
    void continue_path();
    void find_neighbours(int x, int y);
    void identify_successors(SearchNode* current_node, SearchNode* start_node, SearchNode* goal_node);
    SearchNode* get_next_node();
    SearchNode* jump(int c_x, int c_y, int d_x, int d_y, SearchNode* start, SearchNode* goal);
    math::Vector2f get_next_position(math::Vector2f current_position);
};
} // namespace ai
#endif // AI_PATH_FINDER_HPP
