#ifndef LOGIC_AFTERMATCH_STATE
#define LOGIC_AFTERMATCH_STATE

#include <memory>
#include <string>

#include "state.hpp"
#include "../ui/button.hpp"
#include "../ui/font.hpp"

namespace logic {

class Game;

enum MatchResult
{
    MATCH_RESULT_WIN,
    MATCH_RESULT_LOSS
};

class AftermatchState final : public State
{
public:
    std::string    m_result_text;
    math::Vector2f m_result_text_position;
    ui::Font*      m_result_text_font;

    ui::Font*                   m_to_menu_button_font;
    std::unique_ptr<ui::Button> m_to_menu_button;

public:
     AftermatchState(Game& g, MatchResult const res);
    ~AftermatchState();

    AftermatchState            (AftermatchState const&) = delete;
    AftermatchState& operator= (AftermatchState const&) = delete;

    void on_enter () override;
    void on_exit  () override;

    void update(std::chrono::milliseconds const dt) override;
    void render() override;
};

} // namespace logic

#endif // LOGIC_AFTERMATCH_STATE
