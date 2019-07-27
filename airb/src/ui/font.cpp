#include "font.hpp"

#include <algorithm>
#include <stdexcept>

#include "../ext/freetype.hpp"

namespace ui {

Font::Font(ext::FreeType const& ft, std::string const& path, int const sz)
    : m_path    (path)
    , m_size    (sz)

    , m_characters         ()
    , m_texture            ()
    , m_texture_dimensions ({ 0.0f, 0.0f })
{
    FT_Face f;
    int const err = FT_New_Face(ft.get_ft_library(), path.c_str(), 0, &f);
    if (err == FT_Err_Unknown_File_Format)
        throw std::runtime_error("Support for font format not available.");
    else if (err != 0)
        throw std::runtime_error("Reading font file failed.");
    FT_Set_Pixel_Sizes(f, 0, sz);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    FT_GlyphSlot g = f->glyph;

    for (int i = CHARACTER_LB; i != CHARACTER_UB; ++i)
    {
        try
        {
            load_character(f, i);
        }
        catch (std::runtime_error const&)
        {
            FT_Done_Face(f);
            throw;
        }

        m_texture_dimensions[X] += static_cast<float>(g->bitmap.width);
        m_texture_dimensions[Y] = std::max(m_texture_dimensions[Y],
                                           static_cast<float>(g->bitmap.rows));
    }

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RED,
        static_cast<GLsizei>(m_texture_dimensions[X]), static_cast<GLsizei>(m_texture_dimensions[Y]),
        0, GL_RED, GL_UNSIGNED_BYTE, nullptr
    );
    glGenerateMipmap(GL_TEXTURE_2D);

    float x = 0.0f;

    for (int i = CHARACTER_LB; i != CHARACTER_UB; ++i)
    {
        try
        {
            load_character(f, i);
        }
        catch (std::runtime_error const&)
        {
            glDeleteTextures(1, &m_texture);
            FT_Done_Face(f);
            throw;
        }

        Character& ch = m_characters[i];
        ch.dimensions = math::Vector2f({ static_cast<float>(g->bitmap.width), 
                                         static_cast<float>(g->bitmap.rows) });
        ch.bearing    = math::Vector2f({ static_cast<float>(g->bitmap_left),
                                         static_cast<float>(g->bitmap_top) });
        ch.advance    = static_cast<float>(g->advance.x >> 6) + 2.0f;

        ch.uv[ch.UV_LEFT]   = (x + 1.0f)                    / m_texture_dimensions[X];
        ch.uv[ch.UV_RIGHT]  = (x - 1.0f + ch.dimensions[X]) / m_texture_dimensions[X];
        ch.uv[ch.UV_BOTTOM] =      1.0f                     / m_texture_dimensions[Y];
        ch.uv[ch.UV_TOP]    = (  - 1.0f + ch.dimensions[Y]) / m_texture_dimensions[Y];

        glTexSubImage2D(
            GL_TEXTURE_2D, 0,
            static_cast<GLint>(x), 0,
            g->bitmap.width, g->bitmap.rows,
            GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer
        );

        x += ch.dimensions[X];
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    FT_Done_Face(f);
}

Font::~Font()
{
    glDeleteTextures(1, &m_texture);
}

void Font::load_character(FT_Face f, int const ch)
{
    if (FT_Load_Char(f, ch, FT_LOAD_RENDER) != 0)
        throw std::runtime_error(
            "Loading character " + std::to_string(static_cast<char>(ch)) + " failed."
        );
}

float Font::get_text_width(std::string const& txt) const
{
    float curr_w = 0.0f;
    float max_w  = 0.0f;

    for (auto i = txt.begin(); i != txt.end(); ++i)
    {
        if (*i == '\n')
        {
            curr_w = 0.0f;
            continue;
        }

        curr_w += m_characters[*i].advance;
        if (curr_w > max_w)
            max_w = curr_w;
    }

    return max_w;
}

} // namespace ui
