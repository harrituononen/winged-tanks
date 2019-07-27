#include "glfw.hpp"

#include <stdexcept>

#include <GLFW/glfw3.h>

#include "../utilities/debug.hpp"

namespace ext {

GLFW::GLFW()
{
    if (!glfwInit())
        throw std::runtime_error("GLFW initialization failed.");

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    utilities::Debug::log("GLFW initialized.");
}

GLFW::~GLFW()
{
    glfwTerminate();

    utilities::Debug::log("GLFW destroyed.");
}

} // namespace ext
