#ifndef GRAPHICS_IMAGE_HPP
#define GRAPHICS_IMAGE_HPP

#include <fstream>
#include <string>

#include <glew.h>

#include "../math/matrix.hpp"

namespace graphics {

class Image final
{
private:
    std::string const m_path;
    GLubyte*          m_image_data;
    GLuint            m_bits_per_pixel;
    GLint             m_type;
    math::Vector2i    m_dimensions;

public:
    explicit  Image(std::string const& path);
             ~Image();

    Image            (Image const&) = delete;
    Image& operator= (Image const&) = delete;

private:
    void load_tga              (std::string const& path);
    void load_uncompressed_tga (std::string const& path,
                                std::ifstream& f);

public:
    inline GLubyte*              get_image_data     () const;
    inline GLuint                get_bits_per_pixel () const;
    inline GLint                 get_type           () const;
    inline math::Vector2i const& get_dimensions     () const;
};

} // namespace graphics

#include "image.inl"

#endif // GRAPHICS_IMAGE_HPP
