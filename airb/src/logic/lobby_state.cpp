#include "lobby_state.hpp"

#include "game.hpp"
#include "level_data.hpp"
#include "../graphics/render_tools.hpp"
#include "../graphics/texture.hpp"

namespace logic {

LobbyState::LobbyState(Game& g)
    : State (g)

    , m_selected_level (0)

    , m_font           (nullptr)
    , m_button_texture (m_texture_holder.get(graphics::Textures::UI))
    , m_buttons        ()
    , m_name_input     (nullptr)

    , m_thumbnail_sprite        (nullptr)
    , m_thumbnail_text_position ({ 0.0f,
                                   static_cast<float>(
                                       Game::TARGET_DIMENSIONS[Y] / 2
                                     - THUMBNAIL_DIMENSIONS[Y] / 2
                                     - BUTTON_FONT_SIZE - 10
                                   ) })

    , m_music (m_music_holder.get(audio::MusicThemes::LOBBY))
{
    m_font_holder.load(
        ui::Fonts::BASIC_SANS,
        BUTTON_FONT_SIZE,
        std::make_unique<ui::Font>(
            m_game.get_freetype(), "res/fonts/riffic_free.ttf", BUTTON_FONT_SIZE
        )
    );

    m_font = &m_font_holder.get(ui::Fonts::BASIC_SANS, BUTTON_FONT_SIZE);
    int const btn_h         = 100 + BUTTON_FONT_SIZE;
    int const start_btn_w   = 400;
    int const rl_btn_w      = 100;
    int const rl_btn_offset = THUMBNAIL_DIMENSIONS[X] / 2;

    m_buttons[BUTTON_START] = std::make_unique<ui::Button>(
        *m_font,
        "Start",
        math::Vector4f::one(),
        m_button_texture,
        math::Vector2i::zero(),
        m_button_texture.get_dimensions(),
        math::Vector2i({ Game::TARGET_DIMENSIONS[X] - start_btn_w - 60, 60 }),
        math::Vector2i({ start_btn_w, btn_h })
    );

    m_buttons[BUTTON_LEFT] = std::make_unique<ui::Button>(
        *m_font,
        "<",
        math::Vector4f::one(),
        m_button_texture,
        math::Vector2i::zero(),
        m_button_texture.get_dimensions(),
        math::Vector2i({ Game::TARGET_DIMENSIONS[X] / 2 - rl_btn_offset - rl_btn_w,
                         Game::TARGET_DIMENSIONS[Y] / 2 - btn_h / 2 }),
        math::Vector2i({ rl_btn_w, btn_h })
    );

    m_buttons[BUTTON_RIGHT] = std::make_unique<ui::Button>(
        *m_font,
        ">",
        math::Vector4f::one(),
        m_button_texture,
        math::Vector2i::zero(),
        m_button_texture.get_dimensions(),
        math::Vector2i({ Game::TARGET_DIMENSIONS[X] / 2 + rl_btn_offset,
                         Game::TARGET_DIMENSIONS[Y] / 2 - btn_h / 2}),
        math::Vector2i({ rl_btn_w, btn_h })
    );

    m_name_input = std::make_unique<ui::InputField>(
        *m_font, DEFAULT_PLAYER_NAMES[rand() % NUM_DEFAULT_PLAYER_NAMES], 12,
        math::Vector2f::one() * 60.0f,
        std::chrono::milliseconds(750),
        "Name:"
    );

    m_texture_holder.load(
        graphics::Textures::LEVEL_THUMBNAILS,
        std::make_unique<graphics::Texture>("res/textures/levels/thumbnails.tga")
    );

    m_thumbnail_sprite = std::make_unique<graphics::Sprite>(
        m_texture_holder.get(graphics::Textures::LEVEL_THUMBNAILS),
        math::Vector2i::zero(),
        THUMBNAIL_SOURCE_DIMENSIONS,
        Game::TARGET_DIMENSIONS / 2,
        THUMBNAIL_DIMENSIONS,
        math::Vector2f({ 0.5f, 0.5f })
    );

    update_thumbnail();
}

LobbyState::~LobbyState()
{
    for (int i = 0; i < M_NUM_BUTTONS; ++i)
        m_buttons[i] = nullptr;
    m_name_input = nullptr;
    m_font = nullptr;
    m_font_holder.destroy(ui::Fonts::BASIC_SANS, BUTTON_FONT_SIZE);

    m_thumbnail_sprite = nullptr;
    m_texture_holder.destroy(graphics::Textures::LEVEL_THUMBNAILS);
}

void LobbyState::on_enter()
{
    graphics::RenderTools::set_clear_color(0.0f, 0.25f, 0.5f, 1.0f);
    m_music.play();
}

void LobbyState::on_exit()
{
    m_music.stop();
}

void LobbyState::update(std::chrono::milliseconds const dt)
{
    if (m_keyboard.was_key_pressed(GLFW_KEY_F4))
    {
        m_game.pop_state();
        return;
    }

    m_name_input->update(dt, m_keyboard);

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
                if (i == BUTTON_LEFT)
                {
                    m_selected_level = m_selected_level - 1 < 0 
                                     ? NUM_LEVELS - 1
                                     : m_selected_level - 1;
                    update_thumbnail();
                }
                else if (i == BUTTON_RIGHT)
                {
                    m_selected_level = m_selected_level + 1 > NUM_LEVELS - 1
                                     ? 0
                                     : m_selected_level + 1;
                    update_thumbnail();
                }
                else if (i == BUTTON_START)
                {
                    m_game.start_single_player_game(LEVEL_IDS[m_selected_level],
                                                    LEVEL_PATHS[m_selected_level]);
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

void LobbyState::render()
{
    m_batch_renderer.begin();
    m_batch_renderer.submit(*m_thumbnail_sprite);
    for (int i = 0; i < M_NUM_BUTTONS; ++i)
        m_batch_renderer.submit(m_buttons[i]->get_sprite());
    m_batch_renderer.end();
    m_batch_renderer.flush();

    for (int i = 0; i < M_NUM_BUTTONS; ++i)
        m_buttons[i]->render_text(m_font_renderer);
    m_font_renderer.render_shadowed_text(
        *m_font,
        LEVEL_NAMES[m_selected_level],
        m_thumbnail_text_position,
        math::Vector2f({ 1.0f, -1.0f })
    );
    m_name_input->render_text(m_font_renderer);
}

void LobbyState::update_thumbnail()
{
    m_thumbnail_sprite->set_source_position(math::Vector2i({
        0, static_cast<int>(m_selected_level) * THUMBNAIL_SOURCE_DIMENSIONS[Y]
    }));

    float const txt_w = m_font->get_text_width(LEVEL_NAMES[m_selected_level]);
    m_thumbnail_text_position[X] = static_cast<float>(Game::TARGET_DIMENSIONS[X] / 2)
                                 - txt_w / 2.0f;
}

} // namespace logic
