#ifndef UI_FONT_RENDERER_HPP
#define UI_FONT_RENDERER_HPP

#include <glew.h>

#include "../graphics/shader.hpp"
#include "../math/matrix.hpp"

namespace ui {

class Font;

class FontRenderer final
{
private:
    GLuint           m_vertex_array;
    GLuint           m_vertex_buffer;
    GLuint           m_index_buffer;
    graphics::Shader m_shader;

    GLsizeiptr m_num_elements;
    GLsizeiptr m_num_characters;

public:
     FontRenderer();
    ~FontRenderer();

    FontRenderer            (FontRenderer const&) = delete;
    FontRenderer& operator= (FontRenderer const&) = delete;

    void render_text          (Font const& font, std::string const& txt,
                               math::Vector2f pos,
                               math::Vector4f const& color = math::Vector4f::one());
    void render_shadowed_text (Font const& font, std::string const& txt,
                               math::Vector2f const& pos,
                               math::Vector2f const& shadow_offset,
                               math::Vector4f const& color = math::Vector4f::one(),
                               math::Vector4f const& shadow_color
                                   = math::Vector4f({ 0.0f, 0.0f, 0.0f, 1.0f }));

    inline graphics::Shader& get_shader();
};

} // namespace ui

#include "font_renderer.inl"

#endif // UI_FONT_RENDERER_HPP
