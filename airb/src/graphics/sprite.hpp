#ifndef GRAPHICS_SPRITE_HPP
#define GRAPHICS_SPRITE_HPP

#include <array>

#include <glew.h>

#include "texture.hpp"
#include "../math/matrix.hpp"

namespace graphics {

class Sprite
{
protected:
    Texture&       m_texture;
    math::Vector2i m_source_position;
    math::Vector2i m_source_dimensions;
    math::Vector2i m_position;
    math::Vector2i m_dimensions;
    math::Vector2f m_anchor;
    math::Vector2f m_tiling;
    GLfloat        m_rotation;

    std::array<math::Vector2f, 4> m_uv;
    std::array<math::Vector4f, 4> m_colors;

public:
             Sprite(Texture& tex,
                    math::Vector2i const& src_pos, math::Vector2i const& src_dim,
                    math::Vector2i const& dst_pos, math::Vector2i const& dst_dim,
                    math::Vector2f const& anchor = math::Vector2f::zero(),
                    math::Vector2f const& tiling = math::Vector2f::one(),
                    GLfloat const rot = 0.0f,
                    std::array<math::Vector4f, 4> const& colors =
                    {
                        math::Vector4f::one(),
                        math::Vector4f::one(),
                        math::Vector4f::one(),
                        math::Vector4f::one()
                    });
    virtual ~Sprite() = default;

    Sprite            (Sprite const&) = delete;
    Sprite& operator= (Sprite const&) = delete;

protected:
    void update_uv();

public:
           void set_source_position   (math::Vector2i const& src_pos);
    inline void set_source_dimensions (math::Vector2i const& src_dim);
    inline void set_dimensions        (math::Vector2i const& dim);

    inline void set_position(math::Vector2i const& pos);
    inline void set_rotation(GLfloat const rot);

    inline void set_color  (math::Vector4f const& color);
    inline void set_colors (std::array<math::Vector4f, 4> const& colors);

    inline Texture const&        get_texture           () const;
    inline math::Vector2i const& get_position          () const;
    inline math::Vector2i const& get_dimensions        () const;
    inline math::Vector2i const& get_source_position   () const;
    inline math::Vector2i const& get_source_dimensions () const;
    inline math::Vector2f const& get_anchor            () const;
    inline GLfloat               get_rotation          () const;

    inline std::array<math::Vector2f, 4> const& get_uv     () const;
    inline std::array<math::Vector4f, 4> const& get_colors () const;
};

} // namespace graphics

#include "sprite.inl"

#endif // GRAPHICS_SPRITE_HPP
