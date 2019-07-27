#include "sprite.hpp"

namespace graphics {

Sprite::Sprite(Texture& tex,
               math::Vector2i const& src_pos, math::Vector2i const& src_dim,
               math::Vector2i const& dst_pos, math::Vector2i const& dst_dim,
               math::Vector2f const& anchor,
               math::Vector2f const& tiling,
               GLfloat const rot,
               std::array<math::Vector4f, 4> const& colors)
    : m_texture           (tex)
    , m_source_position   (src_pos)
    , m_source_dimensions (src_dim)
    , m_position          (dst_pos)
    , m_dimensions        (dst_dim)
    , m_anchor            (anchor)
    , m_tiling            (tiling)
    , m_rotation          (rot)

    , m_uv     ()
    , m_colors (colors)
{
    update_uv();
}

void Sprite::update_uv()
{
    math::Vector2f const src_posf(m_source_position);
    math::Vector2f const src_dimf(m_source_dimensions);
    math::Vector2f const tex_dimf(m_texture.get_dimensions());

    GLfloat const uv_l =  src_posf[X]                / tex_dimf[X];
    GLfloat const uv_b =  src_posf[Y]                / tex_dimf[Y];
    GLfloat const uv_r = (src_posf[X] + src_dimf[X]) / tex_dimf[X] * m_tiling[X];
    GLfloat const uv_t = (src_posf[Y] + src_dimf[Y]) / tex_dimf[Y] * m_tiling[Y];

    m_uv[0] = math::Vector2f({ uv_l, uv_b });
    m_uv[1] = math::Vector2f({ uv_r, uv_b });
    m_uv[2] = math::Vector2f({ uv_r, uv_t });
    m_uv[3] = math::Vector2f({ uv_l, uv_t });
}

void Sprite::set_source_position(math::Vector2i const& src_pos)
{
    m_source_position = src_pos;
    update_uv();
}

} // namespace graphics
