#include "scoreboard.hpp"

#include <iostream>
#include <string>

#include "font.hpp"
#include "font_renderer.hpp"
#include "../logic/game.hpp"

namespace ui {

namespace
{
    std::string const TITLE_TEXT = "Scoreboard";
}

Scoreboard::Scoreboard(Font const& title_font, Font const& score_font)
    : m_title_font (title_font)
    , m_score_font (score_font)

    , m_title_position ({
          static_cast<float>(logic::Game::TARGET_DIMENSIONS[X] / 2)
        - title_font.get_text_width(TITLE_TEXT) / 2.0f,
          static_cast<float>(logic::Game::TARGET_DIMENSIONS[Y] - title_font.get_size() - 100)
      })

    , m_score_text ("")
    , m_score_text_position ()
{
    update_score();
}

Scoreboard::~Scoreboard()
{ }

void Scoreboard::update_score()
{
    m_score_text = "";

    for (int i = 0; i < 10; ++i)
    {
        m_score_text += "Player "
                      + std::to_string(i + 1)
                      + " : "
                      + std::to_string(i)
                      + " / "
                      + std::to_string(i)
                      + "\n";
    }

    float const text_w = m_score_font.get_text_width(m_score_text);
    m_score_text_position = math::Vector2f({ 
        static_cast<float>(logic::Game::TARGET_DIMENSIONS[X] / 2) - text_w / 2.0f,
        m_title_position[Y] - 100.0f
    });
}

void Scoreboard::render(FontRenderer& rend)
{
    rend.render_shadowed_text(m_title_font, TITLE_TEXT, m_title_position,
                              math::Vector2f({ 1.0f, -1.0f }));
    rend.render_shadowed_text(m_score_font, m_score_text, m_score_text_position,
                              math::Vector2f({ 1.0f, -1.0f }));
}

} // namespace ui
