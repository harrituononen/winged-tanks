#ifndef EXT_FREETYPE_HPP
#define EXT_FREETYPE_HPP

#include <ft2build.h>
#include FT_FREETYPE_H

namespace ext {

class FreeType final
{
private:
    FT_Library m_ft_library;

public:
     FreeType();
    ~FreeType();

    FreeType            (FreeType const&) = delete;
    FreeType& operator= (FreeType const&) = delete;

    inline FT_Library get_ft_library() const;
};

} // namespace ext

#include "freetype.inl"

#endif // EXT_FREETYPE_HPP
