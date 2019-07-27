namespace ui {

inline bool Button::is_point_inside(math::Vector2i const& pt) const
{
    return pt[X] >  m_sprite.get_position()[X] 
        && pt[X] < (m_sprite.get_position()[X] + m_sprite.get_dimensions()[X])
        && pt[Y] >  m_sprite.get_position()[Y] 
        && pt[Y] < (m_sprite.get_position()[Y] + m_sprite.get_dimensions()[Y]);
}

inline graphics::Sprite& Button::get_sprite()
{
    return m_sprite;
}

} // namespace ui
