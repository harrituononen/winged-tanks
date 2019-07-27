#pragma once
#ifndef AI_CAUGHT_STATE_HPP
#define AI_CAUGHT_STATE_HPP

#include <iostream>

#include "ai.hpp"
#include "ai_states.hpp"

namespace ai
{

class CaughtState : public AiStates
{
public:
    CaughtState();
    virtual ~CaughtState() {}
    virtual states get_state() { return ST_CAUGHT; }
    virtual void execute(Ai& ai);
};
} // namespace ai
#endif
