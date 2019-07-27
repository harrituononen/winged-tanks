#ifndef UI_SCOREBOARD_HPP
#define UI_SCOREBOARD_HPP

#include <string>

#include "../math/matrix.hpp"

namespace ui {

class Font;
class FontRenderer;

class Scoreboard final
{
private:
    Font const& m_title_font;
    Font const& m_score_font;

    math::Vector2f m_title_position;

    std::string    m_score_text;
    math::Vector2f m_score_text_position;

public:
     explicit  Scoreboard(Font const& title_font, Font const& score_font);
              ~Scoreboard();

    Scoreboard            (Scoreboard const&) = delete;
    Scoreboard& operator= (Scoreboard const&) = delete;

    void update_score();

    void render(FontRenderer& rend);
};

} // namespace ui

#endif // UI_SCOREBOARD_HPP
