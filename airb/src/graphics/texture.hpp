#ifndef GRAPHICS_TEXTURE_HPP
#define GRAPHICS_TEXTURE_HPP

#include <string>

#include <glew.h>

#include "image.hpp"
#include "../math/matrix.hpp"

namespace graphics {

enum class Textures
{
    MAIN_TITLE,
    MENU_BACKGROUND,
    CAVE,
    CITY,
    JUNGLE,
    DESERT,
    SNOW,
    LEVEL_THUMBNAILS,
    CROSSHAIR,
    GAMEPLAY_ELEMENTS,
    UI
};

int constexpr PLANE_DIMENSIONS      = 1 << 9;
int constexpr TURRET_DIMENSIONS     = 1 << 9;
int constexpr PROJECTILE_DIMENSIONS = 1 << 7;
int constexpr EXPLOSION_DIMENSIONS  = 1 << 7;

int constexpr PLANE_COUNT = 6;

int constexpr PLANE_OFFSET_X      = 0;
int constexpr PLANE_OFFSET_Y      = TURRET_DIMENSIONS;
int constexpr TURRET_OFFSET_X     = 0;
int constexpr TURRET_OFFSET_Y     = 0; 
int constexpr PROJECTILE_OFFSET_X = PLANE_OFFSET_X + PLANE_DIMENSIONS * PLANE_COUNT;
int constexpr PROJECTILE_OFFSET_Y = 0;
int constexpr EXPLOSION_OFFSET_X  = PROJECTILE_OFFSET_X + PROJECTILE_DIMENSIONS;
int constexpr EXPLOSION_OFFSET_Y  = 0;

class Texture final
{
private:
    std::string const m_path;
    GLuint            m_texture;
    Image             m_image;
    math::Vector2i    m_dimensions;

public:
    explicit  Texture(std::string const& path, bool const mipmap = true);
             ~Texture();

    Texture            (Texture const&) = delete;
    Texture& operator= (Texture const&) = delete;

    inline void bind() const;

    inline graphics::Image&      get_image      ();
    inline math::Vector2i const& get_dimensions () const;
};

} // namespace graphics

#include "texture.inl"

#endif // GRAPHICS_TEXTURE_HPP
