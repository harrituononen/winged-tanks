namespace entities {

inline void LevelTerrain::add_update_area(math::Vector2i const& bl, math::Vector2i const& tr)
{
    m_update_areas.emplace_back(std::make_pair(bl, tr));
}

inline void LevelTerrain::clear_pixel(math::Vector2i const& px)
{
    m_image_data[get_pixel_alpha_index(px)] = 0;
}

inline unsigned LevelTerrain::get_pixel_alpha_index(math::Vector2i const& px) const
{
    return (px[Y] * m_dimensions[X] + px[X]) * M_RGBA_SIZE + (M_RGBA_SIZE - 1);
}

inline bool LevelTerrain::is_pixel_solid(math::Vector2i const& px) const
{
    return m_image_data[get_pixel_alpha_index(px)] != 0;
}

inline bool LevelTerrain::is_pixel_inside_terrain_bounds(math::Vector2i const& px) const
{
    return px[X] >= 0 && px[X] < m_dimensions[X]
        && px[Y] >= 0 && px[Y] < m_dimensions[Y];
}

} // namespace entities
