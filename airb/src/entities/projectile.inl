namespace entities {

inline void Projectile::reset_elapsed_time()
{
    m_elapsed_time = std::chrono::milliseconds::zero();
}

inline int Projectile::get_damage()
{
    return m_damage;
}
inline void Projectile::set_damage(int const dmg)
{
    m_damage = dmg;
}

inline void Projectile::set_type(int const type)
{
    m_type = type;
}

inline void Projectile::set_lifetime(std::chrono::milliseconds const& lifetime)
{
    m_lifetime = lifetime;
}

inline void Projectile::set_dimensions(math::Vector2i const& dim)
{
    m_sprite.set_dimensions(dim);
}

inline void Projectile::set_source_dimensions(math::Vector2i const& src_dim)
{
    m_sprite.set_source_dimensions(src_dim);
}

inline void Projectile::set_source_position(math::Vector2i const& src_pos)
{
    m_sprite.set_source_position(src_pos);
}

inline graphics::Sprite const& Projectile::get_sprite() const
{
    return m_sprite;
}

} // namespace entities
