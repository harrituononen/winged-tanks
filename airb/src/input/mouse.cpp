#include "mouse.hpp"

#include "../graphics/window.hpp"
#include "../logic/game.hpp"

namespace input {

Mouse::Mouse(graphics::Window const& win)
    : m_window(win)

    , m_held_buttons     ()
    , m_pressed_buttons  ()
    , m_released_buttons ()

    , m_cursor_position ({ 0.0f, 0.0f })
{ }

void Mouse::reset_frame()
{
    m_pressed_buttons.clear();
    m_released_buttons.clear();
}

void Mouse::register_button_action(int const btn, int const act)
{
    if (act == GLFW_PRESS)
    {
        m_pressed_buttons[btn] = true;
        m_held_buttons[btn]    = true;
    }
    else if (act == GLFW_RELEASE)
    {
        m_released_buttons[btn] = true;
        m_held_buttons[btn]     = false;
    }
}

bool Mouse::is_button_held(int const btn) const
{
    if (m_held_buttons.find(btn) != m_held_buttons.end())
        return m_held_buttons.at(btn);
    return false;
}

bool Mouse::was_button_pressed(int const btn) const
{
    if (m_pressed_buttons.find(btn) != m_pressed_buttons.end())
        return m_pressed_buttons.at(btn);
    return false;
}

bool Mouse::was_button_released(int const btn) const
{
    if (m_released_buttons.find(btn) != m_released_buttons.end())
        return m_released_buttons.at(btn);
    return false;
}

math::Vector2f Mouse::get_cursor_world_position(math::Vector2f const& cam_pos) const
{
    math::Vector2f const win_dimf(m_window.get_dimensions());
    math::Vector2f cposf(m_cursor_position);
    cposf[X] *= logic::Game::TARGET_DIMENSIONS[X] / win_dimf[X];
    cposf[Y] *= logic::Game::TARGET_DIMENSIONS[Y] / win_dimf[Y];
    return cam_pos + cposf;
}

} // namespace input
