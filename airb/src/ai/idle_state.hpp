#pragma once
#ifndef AI_IDLE_STATE_HPP
#define AI_IDLE_STATE_HPP

#include <iostream>

#include "ai.hpp"
#include "ai_states.hpp"

namespace ai
{
    class IdleState : public AiStates
    {
    public:
        IdleState();
        virtual ~IdleState() {}
        virtual states get_state() { return ST_IDLE; }
        virtual void execute(Ai& ai);
    };
} // namespace ai
#endif // AI_IDLE_STATE_HPP