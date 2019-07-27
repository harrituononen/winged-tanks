#ifndef ENTITIES_LEVEL_TERRAIN_HPP
#define ENTITIES_LEVEL_TERRAIN_HPP

#include <cassert>
#include <cstdlib>
#include <utility>
#include <vector>

#include <glew.h>

#include "../graphics/texture.hpp"
#include "../math/matrix.hpp"

namespace entities {

class LevelTerrain final
{
private:
    static int const M_RGBA_SIZE = 4;

    graphics::Texture& m_texture;
    math::Vector2i     m_dimensions;
    GLubyte*           m_image_data;

    std::vector<std::pair<math::Vector2i, math::Vector2i>> m_update_areas;
    std::vector<std::pair<math::Vector2i, int>>            m_circles_to_be_destroyed;

public:
    explicit  LevelTerrain(graphics::Texture& tex);
             ~LevelTerrain() = default;

    LevelTerrain            (LevelTerrain const&) = delete;
    LevelTerrain& operator= (LevelTerrain const&) = delete;

    void update         ();
    void destroy_circle (math::Vector2i const& center, int const r);

private:
    inline void add_update_area (math::Vector2i const& bl, math::Vector2i const& tr);
    inline void clear_pixel     (math::Vector2i const& px);

    inline unsigned get_pixel_alpha_index(math::Vector2i const& px) const;

public:
    inline bool is_pixel_solid                 (math::Vector2i const& px) const;
private:
    inline bool is_pixel_inside_terrain_bounds (math::Vector2i const& px) const;
};

} // namespace entities

#include "level_terrain.inl"

#endif // ENTITIES_LEVEL_TERRAIN_HPP
