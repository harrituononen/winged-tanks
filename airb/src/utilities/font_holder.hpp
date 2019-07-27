#ifndef UTILITIES_FONT_HOLDER_HPP
#define UTILITIES_FONT_HOLDER_HPP

#include <map>
#include <memory>
#include <utility>

#include "../ui/font.hpp"

namespace utilities {

class FontHolder final
{
private:
    std::map<std::pair<ui::Fonts, int>, std::unique_ptr<ui::Font>> m_fonts;

public:
     FontHolder();
    ~FontHolder();

    FontHolder            (FontHolder const&) = delete;
    FontHolder& operator= (FontHolder const&) = delete;

    void load            (ui::Fonts const id, int const sz,
                          std::unique_ptr<ui::Font> font_ptr);
    void destroy         (ui::Fonts const id, int const sz);
    ui::Font& get (ui::Fonts const id, int const sz);
};

} // namespace utilities

#endif // UTILITIES_FONT_HOLDER_HPP
