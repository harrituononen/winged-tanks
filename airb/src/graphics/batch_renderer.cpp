#include "batch_renderer.hpp"

#include <array>
#include <memory>
#include <stdexcept>

#include "render_tools.hpp"
#include "sprite.hpp"
#include "texture.hpp"
#include "../math/matrix.hpp"
#include "../utilities/debug.hpp"

namespace graphics {

namespace
{
    GLsizeiptr const NUM_SPRITES_MAX    = 32000;
    GLsizeiptr const VERTEX_SIZE        = sizeof(GLfloat) * 8;
    GLsizeiptr const SPRITE_SIZE        = VERTEX_SIZE * 4;
    GLsizeiptr const VERTEX_BUFFER_SIZE = NUM_SPRITES_MAX * SPRITE_SIZE;
    GLsizeiptr const NUM_INDICES_MAX    = NUM_SPRITES_MAX * 6;
    GLsizeiptr const INDEX_BUFFER_SIZE  = NUM_INDICES_MAX * sizeof(GLuint);

    enum : GLuint
    {
        POSITION_ATTRIBUTE,
        UV_ATTRIBUTE,
        COLOR_ATTRIBUTE
    };
}

BatchRenderer::BatchRenderer()
    : m_vertex_array  ()
    , m_vertex_buffer ()
    , m_index_buffer  ()
    , m_shader        ("res/shaders/batch")

    , m_num_elements (0)
    , m_num_sprites  (0)

    , m_write_buffer     (nullptr)
    , m_previous_texture (nullptr)
{
    glGenVertexArrays(1, &m_vertex_array);
    glGenBuffers(1, &m_vertex_buffer);
    glGenBuffers(1, &m_index_buffer);

    glBindVertexArray(m_vertex_array);

    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, VERTEX_BUFFER_SIZE, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(POSITION_ATTRIBUTE);
    glEnableVertexAttribArray(UV_ATTRIBUTE);
    glEnableVertexAttribArray(COLOR_ATTRIBUTE);

    glVertexAttribPointer(POSITION_ATTRIBUTE, 2, GL_FLOAT, GL_FALSE, VERTEX_SIZE,
                          reinterpret_cast<GLvoid const*>(0));
    glVertexAttribPointer(UV_ATTRIBUTE, 2, GL_FLOAT, GL_FALSE, VERTEX_SIZE,
                          reinterpret_cast<GLvoid const*>(sizeof(GLfloat) * 2));
    glVertexAttribPointer(COLOR_ATTRIBUTE, 4, GL_FLOAT, GL_FALSE, VERTEX_SIZE,
                          reinterpret_cast<GLvoid const*>(sizeof(GLfloat) * 4));

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

    utilities::Debug::log("Batch renderer created.");
}

BatchRenderer::~BatchRenderer()
{
    glDeleteBuffers(1, &m_index_buffer);
    glDeleteBuffers(1, &m_vertex_buffer);
    glDeleteVertexArrays(1, &m_vertex_array);

    utilities::Debug::log("Batch renderer destroyed.");
}

void BatchRenderer::begin()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    m_write_buffer = static_cast<GLfloat*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
}

void BatchRenderer::end() const
{
    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void BatchRenderer::submit(Sprite const& spr)
{
    static auto force_flush = [=]
    {
        end();
        flush();
        begin();
    };

    if (m_num_sprites == NUM_SPRITES_MAX)
        force_flush();

    Texture const* const spr_tex = &spr.get_texture();
    if (m_previous_texture != nullptr && m_previous_texture != spr_tex)
        force_flush();

    math::Vector2f const posf(spr.get_position());
    math::Vector2f const dimf(spr.get_dimensions());
    math::Vector2f const& anchor = spr.get_anchor();
    math::Vector2f const axd({ anchor[X] * dimf[X], anchor[Y] * dimf[Y] });
    GLfloat const rot = spr.get_rotation();

    std::array<math::Vector2f, 4> const pos_arr =
    {
        math::VectorOperations::rotated_2d(-axd                                    , rot) + posf,
        math::VectorOperations::rotated_2d(-axd + math::Vector2f({ dimf[X], 0.0f }), rot) + posf,
        math::VectorOperations::rotated_2d(-axd + dimf                             , rot) + posf,
        math::VectorOperations::rotated_2d(-axd + math::Vector2f({ 0.0f, dimf[Y] }), rot) + posf
    };

    std::array<math::Vector2f, 4> const& uv     = spr.get_uv();
    std::array<math::Vector4f, 4> const& colors = spr.get_colors();

    for (int i = 0; i < 4; ++i)
    {
        *(m_write_buffer++) = pos_arr[i][X];
        *(m_write_buffer++) = pos_arr[i][Y];
        *(m_write_buffer++) = uv[i][X];
        *(m_write_buffer++) = uv[i][Y];
        *(m_write_buffer++) = colors[i][R];
        *(m_write_buffer++) = colors[i][G];
        *(m_write_buffer++) = colors[i][B];
        *(m_write_buffer++) = colors[i][A];
    }

    m_num_elements = ++m_num_sprites * 6;
    m_previous_texture = spr_tex;
}

void BatchRenderer::flush()
{
    m_shader.use();
    glBindVertexArray(m_vertex_array);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
    m_previous_texture->bind();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_num_elements), GL_UNSIGNED_INT, nullptr);
    RenderTools::unbind_texture();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    RenderTools::unuse_shader();

    m_num_sprites = m_num_elements = 0;
}

} // namespace graphics
