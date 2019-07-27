#include "freetype.hpp"

#include <stdexcept>

#include "../utilities/debug.hpp"

namespace ext {

FreeType::FreeType()
    : m_ft_library()
{
    if (FT_Init_FreeType(&m_ft_library) != 0)
        throw std::runtime_error("FreeType initialization failed.");

    utilities::Debug::log("FreeType initialized.");
}

FreeType::~FreeType()
{
    FT_Done_FreeType(m_ft_library);

    utilities::Debug::log("FreeType destroyed.");
}

} // namespace ext
