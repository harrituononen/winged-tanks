#include "font_holder.hpp"

#include <cassert>

#include "debug.hpp"

namespace utilities {

FontHolder::FontHolder()
    : m_fonts ()
{
    Debug::log("Font holder created.");
}

FontHolder::~FontHolder()
{
    Debug::log("Font holder destroyed.");
}

void FontHolder::load(ui::Fonts const id, int const sz,
                      std::unique_ptr<ui::Font> font_ptr)
{
    if (m_fonts.find(std::make_pair(id, sz))!= m_fonts.end())
        return;
    m_fonts.emplace(std::make_pair(id, sz), std::move(font_ptr));
}

void FontHolder::destroy(ui::Fonts const id, int const sz)
{
    m_fonts.erase(std::make_pair(id, sz));
}

ui::Font& FontHolder::get(ui::Fonts const id, int const sz)
{
    auto itr = m_fonts.find(std::make_pair(id, sz));
    assert(itr != m_fonts.end());
    return *itr->second;
}

} // namespace utilities
