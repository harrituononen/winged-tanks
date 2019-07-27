#ifndef UI_FONT_HPP
#define UI_FONT_HPP

#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <glew.h>

#include "../math/matrix.hpp"

namespace ext {

class FreeType;

} // namespace ext

namespace ui {

enum class Fonts
{
    BASIC_SANS
};

class Font final
{
public:
    static int const CHARACTER_LB = 32;
    static int const CHARACTER_UB = 128;

    struct Character
    {
        enum UV
        {
            UV_LEFT  , UV_RIGHT,
            UV_BOTTOM, UV_TOP
        };

        math::Vector2f dimensions;
        math::Vector2f bearing;

        float advance; // Pixels to beginning of next glyph

        float uv[4];
    };

private:
    std::string const m_path;
    int const         m_size;

    Character      m_characters[128];
    GLuint         m_texture;
    math::Vector2f m_texture_dimensions;

public:
     Font(ext::FreeType const& ft, std::string const& path, int const sz);
    ~Font();

    Font            (Font const&) = delete;
    Font& operator= (Font const&) = delete;

private:
    void load_character(FT_Face f, int const ch);

public:
    float get_text_width(std::string const& txt) const;

    inline int              get_size       () const;
    inline Character const* get_characters () const;
    inline GLuint           get_texture    () const;
};

} // namespace ui

#include "font.inl"

#endif // UI_FONT_HPP
