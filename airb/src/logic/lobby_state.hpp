#ifndef LOGIC_LOBBY_STATE_HPP
#define LOGIC_LOBBY_STATE_HPP

#include <memory>

#include "state.hpp"
#include "../graphics/sprite.hpp"
#include "../graphics/texture.hpp"
#include "../ui/button.hpp"
#include "../ui/input_field.hpp"

namespace logic {

class LobbyState final : public State
{
private:
    static int const M_NUM_BUTTONS = 3;

    enum Button
    {
        BUTTON_LEFT,
        BUTTON_RIGHT,
        BUTTON_START
    };

    int m_selected_level;

    ui::Font*                       m_font;
    graphics::Texture&              m_button_texture;
    std::unique_ptr<ui::Button>     m_buttons[M_NUM_BUTTONS];
    std::unique_ptr<ui::InputField> m_name_input;

    std::unique_ptr<graphics::Sprite>  m_thumbnail_sprite;
    math::Vector2f                     m_thumbnail_text_position;

    audio::Music& m_music;

public:
    explicit  LobbyState(Game& g);
             ~LobbyState();

    LobbyState            (LobbyState const&) = delete;
    LobbyState& operator= (LobbyState const&) = delete;

    void on_enter () override;
    void on_exit  () override;

    void update(std::chrono::milliseconds const dt) override;
    void render() override;

private:
    void update_thumbnail();
};

} // namespace logic

#endif // LOGIC_LOBBY_STATE_HPP
