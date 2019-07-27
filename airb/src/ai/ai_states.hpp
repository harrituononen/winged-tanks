#pragma once
#ifndef AI_AI_STATES_HPP
#define AI_AI_STATES_HPP

namespace ai
{

class Ai;

class AiStates
{
public:
    enum states
    {
        ST_IDLE = 0,
        ST_PATROL = 1,
        ST_ATTACK = 2,
        ST_FLEE = 3,
        ST_CAUGHT = 4,
    };

    virtual states get_state() = 0;
    virtual void execute(Ai& ai) = 0;
    virtual ~AiStates(){}
};
} // namespace ai
#endif // AI_AI_STATES_HPP
