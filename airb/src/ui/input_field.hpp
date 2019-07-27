#ifndef UI_INPUT_FIELD_HPP
#define UI_INPUT_FIELD_HPP

#include <chrono>
#include <cstddef>
#include <string>

#include "../input/keyboard.hpp"
#include "../math/matrix.hpp"

namespace ui {

class Font;
class FontRenderer;

class InputField final
{
    bool                      m_enabled;
    Font const&               m_font;
    std::string               m_value;
    std::string               m_value_with_cursor;
    std::string               m_label;
    size_t                    m_max_length;
    std::chrono::milliseconds m_cursor_blink_time;
    std::chrono::milliseconds m_time_since_blink;
    bool                      m_show_cursor;
    math::Vector2f            m_text_position;
    size_t                    m_cursor_position;

public:
     InputField(Font const& font, std::string const& default_val, size_t const max_len,
                math::Vector2f const& txt_pos,
                std::chrono::milliseconds const blink_t,
                std::string const& label = "");
    ~InputField() = default;

    InputField            (InputField const&) = delete;
    InputField& operator= (InputField const&) = delete;

    void update              (std::chrono::milliseconds const dt, input::Keyboard& kb);
    void render_text         (FontRenderer& ren);
    void input_text          (std::string const& txt);
    void set_cursor_position (size_t cpos);
    void set_enabled         (bool const val);
    void move_cursor_left    ();
    void move_cursor_right   ();
    void erase_character     ();

    bool        is_enabled   () const;
    std::string get_value    () const;
};

} // namespace ui

#endif // UI_INPUT_FIELD_HPP
