#ifndef INPUT_MOUSE_HPP
#define INPUT_MOUSE_HPP

#include <map>

#include "../math/matrix.hpp" // Included before GLFW due to GLEW's restrictions

#include <GLFW/glfw3.h>

namespace graphics {

class Window;

} // namespace graphics

namespace input {

enum Button
{
    BUTTON_FIRE = GLFW_MOUSE_BUTTON_1
};

class Mouse final
{
private:
    graphics::Window const& m_window;

    std::map<int, bool> m_held_buttons;
    std::map<int, bool> m_pressed_buttons;
    std::map<int, bool> m_released_buttons;

    math::Vector2f m_cursor_position;

public:
    explicit  Mouse(graphics::Window const& win);
             ~Mouse() = default;

    Mouse            (Mouse const&) = delete;
    Mouse& operator= (Mouse const&) = delete;

    void register_button_action(int const btn, int const act);

    void reset_frame();

    bool is_button_held      (int const btn) const;
    bool was_button_pressed  (int const btn) const;
    bool was_button_released (int const btn) const;

    inline void set_cursor_position(math::Vector2f const& pos);

           math::Vector2f        get_cursor_world_position (math::Vector2f const& cam_pos) const;
    inline math::Vector2f const& get_cursor_position       () const;
};

} // namespace input

#include "mouse.inl"

#endif // INPUT_MOUSE_HPP
