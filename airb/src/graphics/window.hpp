#ifndef GRAPHICS_WINDOW_HPP
#define GRAPHICS_WINDOW_HPP

#include <string>

#include <glew.h>
#include <GLFW/glfw3.h>

#include "../input/keyboard.hpp"
#include "../input/mouse.hpp"
#include "../math/matrix.hpp"

namespace graphics {

enum class WindowMode
{
    WINDOWED,
    FULLSCREEN
};

class Window final
{
private:
    std::string    m_title;
    math::Vector2i m_dimensions;
    GLFWwindow*    m_glfw_window;

public:
    input::Keyboard keyboard;
    input::Mouse    mouse;

public:
     Window(std::string const& title,
            int const w, int const h,
            WindowMode const wm = WindowMode::WINDOWED);
    ~Window();

    Window            (Window const&) = delete;
    Window& operator= (Window const&) = delete;

public:
    inline void swap_buffers       () const;
    inline void set_title          (std::string const& title);
    inline void set_cursor_visible (bool const c);

private:
    std::string get_window_information() const;

public:
    inline std::string           get_title      () const;
    inline math::Vector2i const& get_dimensions () const;

    inline bool is_open() const;
};

} // namespace graphics

#include "window.inl"

#endif // GRAPHICS_WINDOW_HPP
