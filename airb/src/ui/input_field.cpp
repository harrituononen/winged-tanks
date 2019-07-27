#include "input_field.hpp"

#include "font.hpp"
#include "font_renderer.hpp"

namespace ui {

InputField::InputField(Font const& font, std::string const& default_val, size_t const max_len,
                       math::Vector2f const& txt_pos,
                       std::chrono::milliseconds const blink_t,
                       std::string const& label)
    : m_enabled           (true)
    , m_font              (font)
    , m_value             (default_val)
    , m_value_with_cursor (default_val + "I")
    , m_label             (label + " ")
    , m_max_length        (max_len)
    , m_cursor_blink_time (blink_t)
    , m_time_since_blink  (0)
    , m_show_cursor       (true)
    , m_text_position     (txt_pos)
    , m_cursor_position   (default_val.length())
{ }

void InputField::update(std::chrono::milliseconds const dt, input::Keyboard& kb)
{
    if (!m_enabled)
        return;

    if (kb.was_key_pressed(GLFW_KEY_LEFT))
        move_cursor_left();
    else if (kb.was_key_pressed(GLFW_KEY_RIGHT))
        move_cursor_right();
    else if (kb.was_key_pressed(GLFW_KEY_BACKSPACE))
        erase_character();
    input_text(kb.get_typed_characters());

    m_time_since_blink += dt;
    if (m_time_since_blink >= m_cursor_blink_time)
    {
        m_time_since_blink -= m_cursor_blink_time;
        m_show_cursor = !m_show_cursor;
    }
}

void InputField::render_text(FontRenderer& ren)
{
    if (!m_enabled)
        return;

    ren.render_shadowed_text(m_font, m_label + (m_show_cursor ? m_value_with_cursor : m_value),
                             m_text_position, math::Vector2f({ 1.0f, -1.0f }));
}

void InputField::input_text(std::string const& txt)
{
    if (!m_enabled)
        return;

    size_t const prev_len = m_value.length();
    if (prev_len == m_max_length)
        return;

    size_t const in_len = txt.length();
    if (in_len == 0)
        return;

    if (m_cursor_position == prev_len)
        m_value += txt;
    else
        m_value.insert(m_cursor_position, txt);
    set_cursor_position(m_cursor_position + in_len);
}

void InputField::set_cursor_position(size_t const cpos)
{
    m_cursor_position = cpos;
    m_value_with_cursor = m_value;
    if (cpos == m_value.length())
        m_value_with_cursor += "I";
    else
        m_value_with_cursor.insert(cpos, "I");
    m_time_since_blink = std::chrono::milliseconds(0);
    m_show_cursor = true;
}

void InputField::set_enabled(bool val)
{
    m_enabled = val;
}

void InputField::move_cursor_left()
{
    if (m_cursor_position != 0)
        set_cursor_position(m_cursor_position - 1);
}

void InputField::move_cursor_right()
{
    if (m_cursor_position != m_value.length())
        set_cursor_position(m_cursor_position + 1);
}

void InputField::erase_character()
{
    if (!m_enabled)
        return;

    if (m_cursor_position != 0 && m_value.length() != 0)
    {
        m_value.erase(m_cursor_position - 1, 1);
        move_cursor_left();
    }
}

bool InputField::is_enabled() const
{
    return m_enabled;
}

std::string InputField::get_value() const
{
    return m_value;
}

} // namespace ui
