#include "keyboard.hpp"

#include "../ui/font.hpp"

namespace input {

Keyboard::Keyboard()
    : m_held_keys        ()
    , m_pressed_keys     ()
    , m_released_keys    ()
    , m_typed_characters ("")
{ }

void Keyboard::register_key_action(int const key, int const act)
{
    if (act == GLFW_PRESS)
    {
        m_pressed_keys[key] = true;
        m_held_keys[key]    = true;
    }
    else if (act == GLFW_RELEASE)
    {
        m_released_keys[key] = true;
        m_held_keys[key]     = false;
    }
}

void Keyboard::register_typed_character(unsigned int const chr)
{
    if (static_cast<int>(chr) <  ui::Font::CHARACTER_LB
     || static_cast<int>(chr) >= ui::Font::CHARACTER_UB)
        return;

    m_typed_characters += static_cast<char>(chr);
}

void Keyboard::reset_frame()
{
    m_pressed_keys.clear();
    m_released_keys.clear();
    m_typed_characters = "";
}

bool Keyboard::is_key_held(int const key) const
{
    if (m_held_keys.find(key) != m_held_keys.end())
        return m_held_keys.at(key);
    return false;
}

bool Keyboard::was_key_pressed(int const key) const
{
    if (m_pressed_keys.find(key) != m_pressed_keys.end())
        return m_pressed_keys.at(key);
    return false;
}

bool Keyboard::was_key_released(int const key) const
{
    if (m_released_keys.find(key) != m_released_keys.end())
        return m_released_keys.at(key);
    return false;
}

std::string const& Keyboard::get_typed_characters() const
{
    return m_typed_characters;
}

} // namespace input
