#include "window.hpp"

#include <stdexcept>

#include "render_tools.hpp"
#include "../utilities/debug.hpp"

namespace graphics {

static GLFWwindow* create_glfw_window(std::string const& title,
                                      int const w, int const h,
                                      WindowMode const wm);

static Window* get_window_pointer(GLFWwindow* const win);

static void key_callback             (GLFWwindow* const win,
                                      int const key, int const sc,
                                      int const act, int const mods);
static void mouse_button_callback    (GLFWwindow* const win,
                                      int const btn, int const act,
                                      int const mods);
static void cursor_position_callback (GLFWwindow* const win,
                                      double const x, double y);
static void character_callback       (GLFWwindow* const win,
                                      unsigned const chr);

Window::Window(std::string const& title,
               int const w, int const h,
               WindowMode const wm)
    : m_title       (title)
    , m_dimensions  ({ w, h })
    , m_glfw_window (create_glfw_window(title, w, h, wm))

    , keyboard ()
    , mouse    (*this)
{
    glfwSwapInterval(0);

    glfwSetWindowUserPointer(m_glfw_window, this);
    glfwSetKeyCallback(m_glfw_window, key_callback);
    glfwSetMouseButtonCallback(m_glfw_window, mouse_button_callback);
    glfwSetCursorPosCallback(m_glfw_window, cursor_position_callback);
    glfwSetCharCallback(m_glfw_window, character_callback);

    RenderTools::enable_textures();
    RenderTools::enable_transparency();

    utilities::Debug::log(get_window_information() + " created.");
}

Window::~Window()
{
    glfwDestroyWindow(m_glfw_window);

    utilities::Debug::log(get_window_information() + " destroyed.");
}

static GLFWwindow* create_glfw_window(std::string const& title,
                                      int const w, int const h,
                                      WindowMode const wm)
{
    GLFWmonitor* const mon = wm == WindowMode::FULLSCREEN ? glfwGetPrimaryMonitor() : nullptr;
    GLFWwindow*  const win = glfwCreateWindow(w, h, title.c_str(), mon, nullptr);

    if (win == nullptr)
        throw std::runtime_error("Window creation failed.");

    glfwMakeContextCurrent(win);

    return win;
}

std::string Window::get_window_information() const
{
    return "Window \"" + m_title + "\", "
         + std::to_string(m_dimensions[X]) + "x" + std::to_string(m_dimensions[Y]);
}

static Window* get_window_pointer(GLFWwindow* const win)
{
    return static_cast<Window*>(glfwGetWindowUserPointer(win));
}

static void key_callback(GLFWwindow* const win,
                         int const key, int const sc,
                         int const act, int const mods)
{
    (void) sc;
    (void) mods;

    get_window_pointer(win)->keyboard.register_key_action(key, act);
}

static void mouse_button_callback(GLFWwindow* const win,
                                  int const btn, int const act,
                                  int const mods)
{
    (void) mods;

    get_window_pointer(win)->mouse.register_button_action(btn, act);
}

static void cursor_position_callback(GLFWwindow* const win,
                                     double const x, double y)
{
    Window* const win_ptr = get_window_pointer(win);

    y = static_cast<double>(win_ptr->get_dimensions()[Y]) - y;

    win_ptr->mouse.set_cursor_position(
        math::Vector2f({ static_cast<float>(x), static_cast<float>(y) })
    );
}

static void character_callback(GLFWwindow* const win,
                               unsigned const chr)
{
    get_window_pointer(win)->keyboard.register_typed_character(static_cast<int>(chr));
}

} // namespace graphics
