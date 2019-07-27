#include <algorithm>

namespace graphics {

inline void Sprite::set_dimensions(math::Vector2i const& dim)
{
    m_dimensions = dim;
}

inline void Sprite::set_source_dimensions(math::Vector2i const& src_dim)
{
    m_source_dimensions = src_dim;
}

inline void Sprite::set_position(math::Vector2i const& pos)
{
    m_position = pos;
}

inline void Sprite::set_rotation(GLfloat const rot)
{
    m_rotation = rot;
}

inline void Sprite::set_color(math::Vector4f const& color)
{
    std::fill(m_colors.begin(), m_colors.end(), color);
}

inline void Sprite::set_colors(std::array<math::Vector4f, 4> const& colors)
{
    m_colors = colors;
}

inline Texture const& Sprite::get_texture() const
{
    return m_texture;
}

inline math::Vector2i const& Sprite::get_position() const
{
    return m_position;
}

inline math::Vector2i const& Sprite::get_source_position() const
{
    return m_source_position;
}

inline math::Vector2i const& Sprite::get_dimensions() const
{
    return m_dimensions;
}

inline math::Vector2i const& Sprite::get_source_dimensions() const
{
    return m_source_dimensions;
}

inline math::Vector2f const& Sprite::get_anchor() const
{
    return m_anchor;
}

inline GLfloat Sprite::get_rotation() const
{
    return m_rotation;
}

inline std::array<math::Vector2f, 4> const& Sprite::get_uv() const
{
    return m_uv;
}

inline std::array<math::Vector4f, 4> const& Sprite::get_colors() const
{
    return m_colors;
}

} // namespace graphics
