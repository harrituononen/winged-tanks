#include "attack_state.hpp"

ai::AttackState::AttackState()
{}
void ai::AttackState::execute(Ai& ai)
{
    //std::cout << "Attack state execute " << std::endl;
    ai.chase();
}
