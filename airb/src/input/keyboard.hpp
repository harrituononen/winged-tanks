#ifndef INPUT_KEYBOARD_HPP
#define INPUT_KEYBOARD_HPP

#include <map>

#include <glew.h>
#include <GLFW/glfw3.h>

namespace input {

enum Key
{
    KEY_THRUST     = GLFW_KEY_W,
    KEY_DECELERATE = GLFW_KEY_S,
    KEY_YAW_LEFT   = GLFW_KEY_A,
    KEY_YAW_RIGHT  = GLFW_KEY_D,

    KEY_WEAPON_1        = GLFW_KEY_1,
    KEY_WEAPON_2        = GLFW_KEY_2,
    KEY_WEAPON_3        = GLFW_KEY_3,
    KEY_WEAPON_PREVIOUS = GLFW_KEY_Q,
    KEY_RELOAD          = GLFW_KEY_R
};

class Keyboard final
{
private:
    std::map<int, bool> m_held_keys;
    std::map<int, bool> m_pressed_keys;
    std::map<int, bool> m_released_keys;
    std::string         m_typed_characters;

public:
     Keyboard();
    ~Keyboard() = default;

    Keyboard            (Keyboard const&) = delete;
    Keyboard& operator= (Keyboard const&) = delete;

    void register_key_action      (int const key, int const act);
    void register_typed_character (unsigned int const chr);

    void reset_frame();

    bool is_key_held      (int const key) const;
    bool was_key_pressed  (int const key) const;
    bool was_key_released (int const key) const;

    std::string const& get_typed_characters() const;
};

} // namespace input

#endif // INPUT_KEYBOARD_HPP
