#include "level_terrain.hpp"

#include <algorithm>

#include "../graphics/render_tools.hpp"

namespace entities {

LevelTerrain::LevelTerrain(graphics::Texture& tex)
    : m_texture    (tex)
    , m_dimensions (math::Vector2i({ tex.get_dimensions()[X],
                                     tex.get_dimensions()[Y] / 2 }))
    , m_image_data (tex.get_image().get_image_data())

    , m_update_areas            ()
    , m_circles_to_be_destroyed ()
{
    m_update_areas.reserve(256);
    m_circles_to_be_destroyed.reserve(256);
}

void LevelTerrain::update()
{
    m_texture.bind();
    glPixelStorei(GL_UNPACK_ROW_LENGTH, m_dimensions[X]);

    for (auto const& p : m_circles_to_be_destroyed)
    {
        math::Vector2i const& center = p.first;
        int const r = p.second;

        int x   = r;
        int y   = 0;
        int err = 0;

        int ypy;
        int ymy;
        int ypx;
        int ymx;

        while (x >= y)
        {
            ypy = std::min(center[Y] + y, m_dimensions[Y] - 1);
            ymy = std::max(center[Y] - y, 0);
            ypx = std::min(center[Y] + x, m_dimensions[Y] - 1);
            ymx = std::max(center[Y] - x, 0);

            int const start_x1 = std::max(center[X] - x, 0);
            int const end_x1   = std::min(center[X] + x, m_dimensions[X] - 1);

            for (int i = start_x1; i != end_x1; ++i)
            {
                clear_pixel(math::Vector2i({ i, ypy }));
                clear_pixel(math::Vector2i({ i, ymy }));
            }

            int const start_x2 = std::max(center[X] - y, 0);
            int const end_x2   = std::min(center[X] + y, m_dimensions[X] - 1);

            for (int i = start_x2; i != end_x2; ++i)
            {
                clear_pixel(math::Vector2i({ i, ypx }));
                clear_pixel(math::Vector2i({ i, ymx }));
            }

            err += 1 + 2 * (++y);
            if (2 * (err - x) + 1 > 0)
                err += 1 - 2 * (--x);
        }

        add_update_area(
            math::Vector2i({ std::max(center[X] - r, 0),
                             std::max(center[Y] - r, 0) }),
            math::Vector2i({ std::min(center[X] + r, m_dimensions[X] - 1),
                             std::min(center[Y] + r, m_dimensions[Y] - 1) })
        );
    }

    size_t const num_areas = m_update_areas.size();
    for (size_t i = 0; i < num_areas; ++i)
    {
        auto const& p = m_update_areas[i];

        int const area_w = p.second[X] - p.first[X];
        int const area_h = p.second[Y] - p.first[Y];

        glTexSubImage2D(
            GL_TEXTURE_2D, 0,
            p.first[X], p.first[Y], area_w, area_h,
            GL_RGBA, GL_UNSIGNED_BYTE,
            &m_image_data[(p.first[Y] * m_dimensions[X] + p.first[X]) * M_RGBA_SIZE]
        );
    }

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    graphics::RenderTools::unbind_texture();
    m_update_areas.clear();
    m_circles_to_be_destroyed.clear();
}

void LevelTerrain::destroy_circle(math::Vector2i const& center, int const r)
{
    //assert(is_pixel_inside_terrain_bounds(center));
    assert(r > 0);

    // Hackish last night workaround.
    if (!is_pixel_inside_terrain_bounds(center))
        return;

    m_circles_to_be_destroyed.emplace_back(std::make_pair(center, r));
}

} // namespace entities
