#include "image.hpp"

#include <cstdlib>
#include <cstring>
#include <stdexcept>

#include "../utilities/debug.hpp"

namespace graphics {

Image::Image(std::string const& path)
    : m_path           (path)
    , m_image_data     (nullptr)
    , m_bits_per_pixel ()
    , m_type           ()
    , m_dimensions     ()
{
    utilities::Debug::log("Now loading image " + m_path + ".");

    load_tga(path);

    utilities::Debug::log("Image " + m_path + " loaded.");
}

Image::~Image()
{
    delete[] m_image_data;

    utilities::Debug::log("Image " + m_path + " destroyed.");
}

void Image::load_tga(std::string const& path)
{
    static GLubyte const cmp_header[12] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    GLubyte f_t[12];

    std::ifstream f(path, std::ios::binary);
    if (!f.is_open())
        throw std::runtime_error("Opening file " + path + " failed.");

    char f_t_buf[12];
    f.read(f_t_buf, sizeof f_t);

    for (size_t i = 0; i < sizeof f_t; ++i)
        f_t[i] = static_cast<GLubyte>(f_t_buf[i]);

    if (memcmp(cmp_header, &f_t, sizeof f_t) == 0)
    {
        load_uncompressed_tga(path, f);
        f.close();
        return;
    }

    f.close();
    throw std::runtime_error("Format of file " + path + " not supported.");
}

void Image::load_uncompressed_tga(std::string const& path,
                                  std::ifstream& f)
{
    GLubyte header[6];

    char header_buf[6];
    f.read(header_buf, sizeof header);

    for (size_t i = 0; i < sizeof header; ++i)
        header[i] = static_cast<GLubyte>(header_buf[i]);

    m_dimensions[X] = static_cast<int>(header[1] * 256 + header[0]);
    m_dimensions[Y] = static_cast<int>(header[3] * 256 + header[2]);

    m_bits_per_pixel = header[4];

    if (m_dimensions[X] <= 0
     || m_dimensions[Y] <= 0
     || (m_bits_per_pixel != 24 && m_bits_per_pixel != 32))
    {
        f.close();
        throw std::runtime_error("Header values of TGA file " + path + " were invalid.");
    }

    m_type = m_bits_per_pixel == 24 ? GL_RGB : GL_RGBA;

    GLuint const bytes_per_pixel = m_bits_per_pixel / 8;
    GLuint const img_sz          = bytes_per_pixel
                                 * static_cast<GLuint>(m_dimensions[X])
                                 * static_cast<GLuint>(m_dimensions[Y]);

    char* img_data_buf;
    try
    {
        m_image_data = new GLubyte[img_sz];
        img_data_buf = new char[img_sz];
    }
    catch (std::bad_alloc const&)
    {
        f.close();
        throw std::runtime_error("Failed to allocate memory for image data.");
    }

    f.read(img_data_buf, sizeof(GLubyte) * img_sz);

    for (GLuint i = 0; i < img_sz; ++i)
        m_image_data[i] = static_cast<GLubyte>(img_data_buf[i]);
    delete[] img_data_buf;

    for (GLuint cswap = 0; cswap < img_sz; cswap += bytes_per_pixel)
    {
        m_image_data[cswap]     ^= m_image_data[cswap + 2];
        m_image_data[cswap + 2] ^= m_image_data[cswap];
        m_image_data[cswap]     ^= m_image_data[cswap + 2];
    }
}

} // namespace graphics
