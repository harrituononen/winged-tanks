#include "font_renderer.hpp"

#include <memory>
#include <stdexcept>

#include "font.hpp"
#include "../graphics/render_tools.hpp"
#include "../utilities/debug.hpp"

namespace ui {

namespace
{
    GLsizeiptr const NUM_CHARACTERS_MAX = 4096;
    GLsizeiptr const VERTEX_SIZE        = sizeof(GLfloat) * 4;
    GLsizeiptr const CHARACTER_SIZE     = VERTEX_SIZE * 4;
    GLsizeiptr const VERTEX_BUFFER_SIZE = NUM_CHARACTERS_MAX * CHARACTER_SIZE;
    GLsizeiptr const NUM_INDICES_MAX    = NUM_CHARACTERS_MAX * 6;
    GLsizeiptr const INDEX_BUFFER_SIZE  = NUM_INDICES_MAX * sizeof(GLuint);

    enum : GLuint
    {
        POSITION_ATTRIBUTE,
        UV_ATTRIBUTE
    };
}

FontRenderer::FontRenderer()
    : m_vertex_array  ()
    , m_vertex_buffer ()
    , m_index_buffer  ()
    , m_shader        ("res/shaders/font")

    , m_num_elements   (0)
    , m_num_characters (0)
{
    glGenVertexArrays(1, &m_vertex_array);
    glGenBuffers(1, &m_vertex_buffer);
    glGenBuffers(1, &m_index_buffer);

    glBindVertexArray(m_vertex_array);

    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, VERTEX_BUFFER_SIZE, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(POSITION_ATTRIBUTE);
    glEnableVertexAttribArray(UV_ATTRIBUTE);

    glVertexAttribPointer(POSITION_ATTRIBUTE, 2, GL_FLOAT, GL_FALSE, VERTEX_SIZE,
                          reinterpret_cast<GLvoid const*>(0));
    glVertexAttribPointer(UV_ATTRIBUTE, 2, GL_FLOAT, GL_FALSE, VERTEX_SIZE,
                          reinterpret_cast<GLvoid const*>(sizeof(GLfloat) * 2));

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    try
    {
        std::unique_ptr<GLuint[]> indices(new GLuint[NUM_INDICES_MAX]);
        GLuint idx_offset = 0;
        for (GLsizeiptr i = 0; i < NUM_INDICES_MAX; i += 6)
        {
            indices[i + 0] = idx_offset + 0;
            indices[i + 1] = idx_offset + 1;
            indices[i + 2] = idx_offset + 2;
            indices[i + 3] = idx_offset + 2;
            indices[i + 4] = idx_offset + 3;
            indices[i + 5] = idx_offset + 0;

            idx_offset += 4;
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, INDEX_BUFFER_SIZE, &indices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    catch (std::bad_alloc const&)
    {
        glDeleteBuffers(1, &m_index_buffer);
        glDeleteBuffers(1, &m_vertex_buffer);
        glDeleteVertexArrays(1, &m_vertex_array);
        throw std::runtime_error("Failed to allocate memory for index buffer.");
    }

    glBindVertexArray(0);

    utilities::Debug::log("Font renderer created.");
}

FontRenderer::~FontRenderer()
{
    glDeleteBuffers(1, &m_index_buffer);
    glDeleteBuffers(1, &m_vertex_buffer);
    glDeleteVertexArrays(1, &m_vertex_array);

    utilities::Debug::log("Font renderer destroyed.");
}

void FontRenderer::render_text(Font const& font, std::string const& txt,
                               math::Vector2f pos,
                               math::Vector4f const& color)
{
    m_num_elements = m_num_characters = 0;

    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    GLfloat* buf = static_cast<GLfloat*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
    Font::Character const* chars = font.get_characters();
    float const org_pos_x = pos[X];

    for (auto i = txt.begin(); i != txt.end(); ++i)
    {
        if (*i == '\n')
        {
            pos[X] = org_pos_x;
            pos[Y] -= static_cast<float>(font.get_size()) * 1.25f;
        }

        Font::Character const& ch = chars[*i];

        math::Vector2f const& ch_dim  = ch.dimensions;
        math::Vector2f const& ch_bear = ch.bearing;
        math::Vector2f const ch_pos({ pos[X] + ch_bear[X], pos[Y] - ch_dim[Y] + ch_bear[Y] });

        float const pos_l = ch_pos[X];
        float const pos_r = ch_pos[X] + ch_dim[X];
        float const pos_b = ch_pos[Y] + ch_dim[Y];
        float const pos_t = ch_pos[Y];

        float const& uv_l = ch.uv[ch.UV_LEFT];
        float const& uv_r = ch.uv[ch.UV_RIGHT];
        float const& uv_b = ch.uv[ch.UV_BOTTOM];
        float const& uv_t = ch.uv[ch.UV_TOP];

        *(buf++) = pos_l; *(buf++) = pos_b; *(buf++) = uv_l; *(buf++) = uv_b;
        *(buf++) = pos_r; *(buf++) = pos_b; *(buf++) = uv_r; *(buf++) = uv_b;
        *(buf++) = pos_r; *(buf++) = pos_t; *(buf++) = uv_r; *(buf++) = uv_t;
        *(buf++) = pos_l; *(buf++) = pos_t; *(buf++) = uv_l; *(buf++) = uv_t;

        pos[X] += ch.advance;
        m_num_elements = ++m_num_characters * 6;
    }

    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_shader.use();
    m_shader.set_vector_uniform_4fv("color", color);
    glBindVertexArray(m_vertex_array);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
    glBindTexture(GL_TEXTURE_2D, font.get_texture());
    glDrawElements(GL_TRIANGLES, m_num_elements, GL_UNSIGNED_INT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    graphics::RenderTools::unuse_shader();
}

void FontRenderer::render_shadowed_text(Font const& font, std::string const& txt,
                                        math::Vector2f const& pos,
                                        math::Vector2f const& shadow_offset,
                                        math::Vector4f const& color,
                                        math::Vector4f const& shadow_color)
{
    render_text(font, txt, pos + shadow_offset, shadow_color);
    render_text(font, txt, pos, color);
}

} // namespace ui
