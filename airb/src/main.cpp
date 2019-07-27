#include <iostream>
#include <stdexcept>

#include "logic/game.hpp"
#include "utilities/debug.hpp"
#include "utilities/spawn_point.hpp"

int main()
{
    bool err = false;

    utilities::init_spawn_points();

    try
    {
        utilities::Debug::log("Starting program.");
        logic::Game g;
        g.run();
    }
    catch (std::exception const& ex)
    {
        std::cerr << ex.what() << std::endl;
        err = true;
    }

    utilities::Debug::log("Exiting program.");
    return err ? 1 : 0;
}
