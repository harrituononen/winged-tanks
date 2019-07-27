namespace graphics {

inline GLubyte* Image::get_image_data() const
{
    return m_image_data;
}

inline GLuint Image::get_bits_per_pixel() const
{
    return m_bits_per_pixel;
}

inline GLint Image::get_type() const
{
    return m_type;
}

inline math::Vector2i const& Image::get_dimensions() const
{
    return m_dimensions;
}

} // namespace graphics
