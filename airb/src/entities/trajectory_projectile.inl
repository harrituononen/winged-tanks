namespace entities
{

inline void TrajectoryProjectile::set_position(math::Vector2f const& pos)
{
    m_rigid_body.set_position(pos);
}

inline void TrajectoryProjectile::set_velocity(math::Vector2f const& vel)
{
    m_rigid_body.set_velocity(vel);
}

} // namespace entities
