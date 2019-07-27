#ifndef GRAPHICS_BATCH_RENDERER_HPP
#define GRAPHICS_BATCH_RENDERER_HPP

#include <glew.h>

#include "shader.hpp"
#include "../math/matrix.hpp"

namespace graphics {

class Sprite;
class Texture;

class BatchRenderer final
{
private:
    GLuint m_vertex_array;
    GLuint m_vertex_buffer;
    GLuint m_index_buffer;
    Shader m_shader;

    GLsizeiptr m_num_elements;
    GLsizeiptr m_num_sprites;

    GLfloat*       m_write_buffer;
    Texture const* m_previous_texture;

public:
     BatchRenderer();
    ~BatchRenderer();

    BatchRenderer            (BatchRenderer const&) = delete;
    BatchRenderer& operator= (BatchRenderer const&) = delete;

    void begin  ();
    void end    () const;
    void submit (Sprite const& spr);
    void flush  ();

    inline Shader& get_shader();
};

} // namespace graphics

#include "batch_renderer.inl"

#endif // GRAPHICS_BATCH_RENDERER_HPP
