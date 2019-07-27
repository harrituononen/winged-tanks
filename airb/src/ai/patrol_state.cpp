#include "patrol_state.hpp"

ai::PatrolState::PatrolState()
{}
void ai::PatrolState::execute(Ai& ai)
{
    //std::cout << "Patrol state execute " << std::endl;
    ai.patrol();
    //if (ai.get_goal_reached())
    //{
    //    ai.change_state(ai::AiStates::ST_IDLE);
    //}
}
