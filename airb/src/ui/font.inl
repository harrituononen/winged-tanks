namespace ui {

inline int Font::get_size() const
{
    return m_size;
}

inline Font::Character const* Font::get_characters() const
{
    return m_characters;
}

inline GLuint Font::get_texture() const
{
    return m_texture;
}

} // namespace ui
