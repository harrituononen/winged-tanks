namespace graphics {

inline void Texture::bind() const
{
    glBindTexture(GL_TEXTURE_2D, m_texture);
}

inline graphics::Image& Texture::get_image()
{
    return m_image;
}

inline math::Vector2i const& Texture::get_dimensions() const
{
    return m_dimensions;
}

} // namespace graphics
