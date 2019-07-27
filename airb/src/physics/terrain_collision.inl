namespace physics
{

    inline math::Vector2f const& TerrainCollision::get_collision_point() const
{
    return m_collision_point;
}

    inline math::Vector2f const& TerrainCollision::get_collision_normal() const
    {
        return m_collision_normal;
    }

    inline float TerrainCollision::get_correction() const
    {
        return m_correction;
    }
} // namespace physics