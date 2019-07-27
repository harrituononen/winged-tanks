#include "texture.hpp"

#include "render_tools.hpp"
#include "../utilities/debug.hpp"

namespace graphics {

Texture::Texture(std::string const& path, bool const mipmap)
    : m_path       (path)
    , m_texture    (0)
    , m_image      (path)
    , m_dimensions (m_image.get_dimensions())
{
    glGenTextures(1, &m_texture);

    bind();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLint const fmt = m_image.get_type();
    glTexImage2D(GL_TEXTURE_2D, 0, fmt,
                 m_dimensions[X], m_dimensions[Y], 0, fmt,
                 GL_UNSIGNED_BYTE, m_image.get_image_data());
    if (mipmap)
        glGenerateMipmap(GL_TEXTURE_2D);

    RenderTools::unbind_texture();

    utilities::Debug::log("Texture of " + m_path + " created.");
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_texture);

    utilities::Debug::log("Texture of " + m_path + " destroyed.");
}

} // namespace graphics
