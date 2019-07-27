namespace entities {

inline std::string const& Weapon::get_name() const
{
    return m_name;
}

inline int Weapon::get_index() const
{
    return m_index;
}

inline bool Weapon::is_reloading() const
{
    return m_reloading;
}

inline bool Weapon::is_automatic() const
{
    return m_automatic;
}

inline int Weapon::get_rounds() const
{
    return m_rounds;
}

inline int Weapon::get_magazine_size() const
{
    return m_mag_size;
}

inline void Weapon::set_position(math::Vector2f const& position)
{
    m_sprite.set_position(math::Vector2i(position));
}

inline void Weapon::set_rotation(float const rot)
{
    m_sprite.set_rotation(rot);
}

inline math::Vector2f Weapon::get_turret_position() const
{
    return math::Vector2f(m_sprite.get_position());
}

inline float Weapon::get_rotation() const
{
    return m_sprite.get_rotation();
}

inline std::chrono::milliseconds Weapon::get_reload_time() const
{
    return m_reload_time;
}

inline graphics::Sprite& Weapon::get_sprite()
{
    return m_sprite;
}

} // namespace entities
