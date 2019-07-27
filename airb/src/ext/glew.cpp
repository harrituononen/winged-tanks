#include "glew.hpp"

#include <stdexcept>

#include <glew.h>

#include "../utilities/debug.hpp"

namespace ext {

GLEW::GLEW()
{
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
        throw std::runtime_error("GLEW initialization failed.");

    utilities::Debug::log("GLEW initialized.");
}

GLEW::~GLEW()
{
    utilities::Debug::log("GLEW should not be used anymore.");
}

} // namespace ext
