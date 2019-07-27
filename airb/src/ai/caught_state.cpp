#include "caught_state.hpp"

ai::CaughtState::CaughtState()
{}
void ai::CaughtState::execute(Ai& ai)
{
    std::cout << "Caught state execute " << std::endl;
    ai.caught();
}
