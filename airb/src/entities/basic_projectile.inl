namespace entities
{

inline void BasicProjectile::set_position(math::Vector2f const& pos)
{
    m_position = pos;
}

inline void BasicProjectile::set_velocity(math::Vector2f const& vel)
{
    m_velocity = vel;
}

} // namespace entities
