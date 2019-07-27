#ifndef LOGIC_TITLE_STATE_HPP
#define LOGIC_TITLE_STATE_HPP

#include <memory>

#include "state.hpp"
#include "../graphics/sprite.hpp"
#include "../graphics/texture.hpp"
#include "../ui/button.hpp"
#include "../ui/font.hpp"
#include "../ui/input_field.hpp"

namespace logic {

class TitleState final : public State
{
private:
    static int const M_NUM_BUTTONS = 4;

    enum Button
    {
        BUTTON_QUIT,
        BUTTON_MULTIPLAY_JOIN,
        BUTTON_MULTIPLAY_HOST,
        BUTTON_SINGLEPLAY
    };

    std::unique_ptr<graphics::Sprite> m_title_sprite;
    std::unique_ptr<graphics::Sprite> m_background_sprite;

    graphics::Texture&              m_button_texture;
    std::unique_ptr<ui::Button>     m_buttons[M_NUM_BUTTONS];
    std::unique_ptr<ui::InputField> m_host_ip_input;

    audio::Music& m_music;

public:
    explicit  TitleState(Game& g);
             ~TitleState();

    TitleState            (TitleState const&) = delete;
    TitleState& operator= (TitleState const&) = delete;

    void on_enter () override;
    void on_exit  () override;

    void update(std::chrono::milliseconds const dt) override;
    void render() override;
};

} // namespace logic

#endif // LOGIC_TITLE_STATE_HPP
