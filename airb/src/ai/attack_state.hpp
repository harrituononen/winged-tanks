#pragma once
#ifndef AI_ATTACK_STATE_HPP
#define AI_ATTACK_STATE_HPP

#include <iostream>

#include "ai.hpp"
#include "ai_states.hpp"

namespace ai
{

class AttackState : public AiStates
{
public:
    AttackState();
    virtual ~AttackState() {}
    virtual states get_state() { return ST_ATTACK; }
    virtual void execute(Ai& ai);
};
} // namespace ai
#endif
