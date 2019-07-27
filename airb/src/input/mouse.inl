namespace input {

inline void Mouse::set_cursor_position(math::Vector2f const& pos)
{
    m_cursor_position = pos;
}

inline math::Vector2f const& Mouse::get_cursor_position() const
{
    return m_cursor_position;
}

} // namespace input
