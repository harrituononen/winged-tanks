#pragma once
#ifndef AI_PATROL_STATE_HPP
#define AI_PATROL_STATE_HPP

#include <iostream>

#include "ai.hpp"
#include "ai_states.hpp"

namespace ai
{
    
class PatrolState : public AiStates
{
public:
    PatrolState();
    virtual ~PatrolState(){}
    virtual states get_state() { return ST_PATROL; }
    virtual void execute(Ai& ai);
};
} // namespace ai
#endif // AI_PATROL_STATE_HPP
