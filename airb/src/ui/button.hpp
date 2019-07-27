#ifndef UI_BUTTON_HPP
#define UI_BUTTON_HPP

#include <string>

#include "../graphics/sprite.hpp"
#include "../math/matrix.hpp"

namespace graphics {

class Texture;

} // namespace graphics

namespace ui {

class Font;
class FontRenderer;

class Button
{
protected:
    std::string      m_text;
    Font const&      m_font;
    graphics::Sprite m_sprite;
    math::Vector2f   m_text_position;
    math::Vector4f   m_text_color;

public:
             Button(Font const& font, std::string const& txt,
                    math::Vector4f const& txt_color,
                    graphics::Texture& tex,
                    math::Vector2i const& src_pos, math::Vector2i const& src_dim,
                    math::Vector2i const& dst_pos, math::Vector2i const& dst_dim);
    virtual ~Button() = default;

    Button            (Button const&) = delete;
    Button& operator= (Button const&) = delete;

    void render_text(FontRenderer& ren);

    inline bool is_point_inside(math::Vector2i const& pt) const;

    inline graphics::Sprite& get_sprite();
};

} // namespace ui

#include "button.inl"

#endif // UI_BUTTON_HPP
