#include "title_state.hpp"

#include <algorithm>

#include "game.hpp"
#include "lobby_state.hpp"
#include "client_lobby_state.hpp"
#include "server_lobby_state.hpp"
#include "../graphics/render_tools.hpp"

namespace logic {

namespace
{
    int const BUTTON_FONT_SIZE = 40;
}

TitleState::TitleState(Game& g)
    : State (g)

    , m_title_sprite      (nullptr)
    , m_background_sprite (nullptr)

    , m_button_texture (m_texture_holder.get(graphics::Textures::UI))
    , m_buttons        ()
    
    , m_music (m_music_holder.get(audio::MusicThemes::MAIN_MENU))
{
    m_texture_holder.load(
        graphics::Textures::MAIN_TITLE,
        std::make_unique<graphics::Texture>("res/textures/main_title.tga")
    );

    graphics::Texture& title_tex = m_texture_holder.get(graphics::Textures::MAIN_TITLE);

    m_title_sprite = std::make_unique<graphics::Sprite>(
        title_tex,
        math::Vector2i::zero(),
        title_tex.get_dimensions(),
        Game::TARGET_DIMENSIONS - math::Vector2i::one() * 50,
        title_tex.get_dimensions() / 3,
        math::Vector2f::one()
    );

    graphics::Texture& bg_tex = m_texture_holder.get(graphics::Textures::MENU_BACKGROUND);

    m_background_sprite = std::make_unique<graphics::Sprite>(
        bg_tex,
        math::Vector2i::zero(),
        bg_tex.get_dimensions(),
        math::Vector2i::zero(),
        bg_tex.get_dimensions()
    );

    m_font_holder.load(
        ui::Fonts::BASIC_SANS,
        BUTTON_FONT_SIZE,
        std::make_unique<ui::Font>(
            m_game.get_freetype(), "res/fonts/riffic_free.ttf", BUTTON_FONT_SIZE
        )
    );

    ui::Font const& btn_font = m_font_holder.get(ui::Fonts::BASIC_SANS, BUTTON_FONT_SIZE);
    for (int i = 0; i < M_NUM_BUTTONS; ++i)
    {
        static std::string const BUTTON_TEXTS[] =
        {
            "Quit",
            "Join game",
            "Host game",
            "Single player"
        };

        static math::Vector2i const BUTTON_DIMENSIONS({ 400, BUTTON_FONT_SIZE + 40 });

        auto& btn = m_buttons[i];
        btn = std::make_unique<ui::Button>(
            btn_font,
            BUTTON_TEXTS[i],
            math::Vector4f::one(),
            m_button_texture,
            math::Vector2i::zero(), m_button_texture.get_dimensions(),
            math::Vector2i({ 60, (BUTTON_DIMENSIONS[Y] + 20) * i + 60 }), BUTTON_DIMENSIONS
        );
    }

    m_host_ip_input = std::make_unique<ui::InputField>(btn_font, network::SERVER_DEFAULT_ADDR, 15,
                                                       math::Vector2f({ 480.0f, 270.0f }),
                                                       std::chrono::milliseconds(750),
                                                       "Host ip:");
    m_host_ip_input->set_enabled(false);
}

TitleState::~TitleState()
{
    m_title_sprite = nullptr;
    m_texture_holder.destroy(graphics::Textures::MAIN_TITLE);
    for (int i = 0; i < M_NUM_BUTTONS; ++i)
        m_buttons[i] = nullptr;
    m_font_holder.destroy(ui::Fonts::BASIC_SANS, BUTTON_FONT_SIZE);
}

void TitleState::on_enter()
{
    graphics::RenderTools::set_clear_color(0.0f, 0.25f, 0.5f, 1.0f);
    //m_music.play();
}

void TitleState::on_exit()
{
    m_music.stop();
}

void TitleState::update(std::chrono::milliseconds const dt)
{
    m_host_ip_input->update(dt, m_keyboard);

    if (m_host_ip_input->is_enabled() && m_keyboard.was_key_pressed(GLFW_KEY_ENTER))
    {
        m_game.push_state(std::make_unique<ClientLobbyState>(
            m_game,
            m_host_ip_input->get_value(),
            network::SERVER_DEFAULT_PORT)
        );
        return;
    }

    for (int i = 0; i < M_NUM_BUTTONS; ++i)
    {
        auto& btn = m_buttons[i];

        bool const hovered = btn->is_point_inside(
            math::Vector2i(m_mouse.get_cursor_world_position(math::Vector2f::zero()))
        );
        if (hovered)
        {
            btn->get_sprite().set_color(math::Vector4f({ 0.0f, 0.0f, 0.0f, 1.0f }));
            if (m_mouse.was_button_pressed(GLFW_MOUSE_BUTTON_1))
            {
                switch (i)
                {
                case BUTTON_SINGLEPLAY:
                    m_game.push_state(std::make_unique<LobbyState>(m_game));
                    return;
                case BUTTON_MULTIPLAY_HOST:
                    m_game.push_state(std::make_unique<ServerLobbyState>(
                        m_game,
                        network::SERVER_DEFAULT_PORT)
                    );
                    return;
                case BUTTON_MULTIPLAY_JOIN:
                    if (!m_host_ip_input->is_enabled())
                    {
                        m_host_ip_input->set_enabled(true);
                    }
                    else
                    {
                        m_game.push_state(std::make_unique<ClientLobbyState>(
                            m_game,
                            m_host_ip_input->get_value(),
                            network::SERVER_DEFAULT_PORT)
                        );
                    }
                    return;
                case BUTTON_QUIT:
                    m_game.stop();
                    return;
                default:
                    return;
                }
            }
        }
        else
        {
            btn->get_sprite().set_color(math::Vector4f::one());
        }
    }
}

void TitleState::render()
{
    m_batch_renderer.begin();
    m_batch_renderer.submit(*m_background_sprite);
    m_batch_renderer.submit(*m_title_sprite);
    for (int i = 0; i < M_NUM_BUTTONS; ++i)
        m_batch_renderer.submit(m_buttons[i]->get_sprite());
    m_batch_renderer.end();
    m_batch_renderer.flush();

    for (int i = 0; i < M_NUM_BUTTONS; ++i)
        m_buttons[i]->render_text(m_font_renderer);

    m_host_ip_input->render_text(m_font_renderer);
}

} // namespace logic
