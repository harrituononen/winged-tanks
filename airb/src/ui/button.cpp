#include "button.hpp"

#include "../graphics/texture.hpp"
#include "../ui/font.hpp"
#include "../ui/font_renderer.hpp"

namespace ui {

Button::Button(Font const& font, std::string const& txt,
               math::Vector4f const& txt_color,
               graphics::Texture& tex,
               math::Vector2i const& src_pos, math::Vector2i const& src_dim,
               math::Vector2i const& dst_pos, math::Vector2i const& dst_dim)
    : m_text          (txt)
    , m_font          (font)
    , m_sprite        (tex, src_pos, src_dim, dst_pos, dst_dim)
    , m_text_position (math::Vector2f({ 
                           static_cast<float>(dst_pos[X]) 
                         + static_cast<float>(dst_dim[X] - font.get_text_width(txt)) / 2.0f,
                           static_cast<float>(dst_pos[Y])
                         + static_cast<float>(dst_dim[Y] - font.get_size()) / 2.0f
                       }))
    , m_text_color    (txt_color)
{ }

void Button::render_text(FontRenderer& ren)
{
    ren.render_shadowed_text(m_font, m_text, m_text_position, math::Vector2f({ 1.0f, -1.0f }));
}

} // namespace ui
