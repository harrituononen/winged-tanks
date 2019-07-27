#include "aftermatch_state.hpp"

#include "game.hpp"
#include "../graphics/render_tools.hpp"

namespace logic {

namespace
{
    std::string const RESULT_TEXTS[] =
    {
        "You win!",
        "You lose!"
    };

    int const RESULT_TEXT_FONT_SIZE = 72;
    int const BUTTON_FONT_SIZE      = 50;
}

AftermatchState::AftermatchState(Game& g, MatchResult const res)
    : State (g)

    , m_result_text          (RESULT_TEXTS[res])
    , m_result_text_position ()
    , m_result_text_font     (nullptr)

    , m_to_menu_button_font (nullptr)
    , m_to_menu_button      (nullptr)
{
    m_font_holder.load(
        ui::Fonts::BASIC_SANS,
        RESULT_TEXT_FONT_SIZE,
        std::make_unique<ui::Font>(
            m_game.get_freetype(), "res/fonts/riffic_free.ttf", RESULT_TEXT_FONT_SIZE
        )
    );

    m_font_holder.load(
        ui::Fonts::BASIC_SANS,
        BUTTON_FONT_SIZE,
        std::make_unique<ui::Font>(
            m_game.get_freetype(), "res/fonts/riffic_free.ttf", BUTTON_FONT_SIZE
        )
    );

    m_result_text_font    = &m_font_holder.get(ui::Fonts::BASIC_SANS, RESULT_TEXT_FONT_SIZE);
    m_to_menu_button_font = &m_font_holder.get(ui::Fonts::BASIC_SANS, BUTTON_FONT_SIZE);

    m_result_text_position[X] = static_cast<float>(Game::TARGET_DIMENSIONS[X] / 2)
                              - m_result_text_font->get_text_width(m_result_text) / 2.0f;
    m_result_text_position[Y] = static_cast<float>(Game::TARGET_DIMENSIONS[Y] / 2)
                              - static_cast<float>(m_result_text_font->get_size() / 2);

    static std::string const btn_txt = "Back to main menu";
    int const btn_txt_w = static_cast<int>(m_to_menu_button_font->get_text_width(btn_txt));
    static math::Vector2i const BUTTON_DIMENSIONS({ btn_txt_w + 100, BUTTON_FONT_SIZE + 100 });
    graphics::Texture& tex = m_texture_holder.get(graphics::Textures::UI);
    m_to_menu_button = std::make_unique<ui::Button>(
        *m_to_menu_button_font,
        btn_txt,
        math::Vector4f::one(),
        tex,
        math::Vector2i::zero(),
        tex.get_dimensions(),
        math::Vector2i({ Game::TARGET_DIMENSIONS[X] - BUTTON_DIMENSIONS[X] - 20, 20 }),
        BUTTON_DIMENSIONS
    );
}

AftermatchState::~AftermatchState()
{
    m_result_text_font    = nullptr;
    m_to_menu_button      = nullptr;
    m_to_menu_button_font = nullptr;
    m_font_holder.destroy(ui::Fonts::BASIC_SANS, RESULT_TEXT_FONT_SIZE);
    m_font_holder.destroy(ui::Fonts::BASIC_SANS, BUTTON_FONT_SIZE);
}

void AftermatchState::on_enter()
{
    graphics::RenderTools::set_clear_color(0.0f, 0.25f, 0.5f, 1.0f);
}

void AftermatchState::on_exit()
{ }

void AftermatchState::update(std::chrono::milliseconds const dt)
{
    (void) dt;

    bool const hovered = m_to_menu_button->is_point_inside(
        math::Vector2i(m_mouse.get_cursor_world_position(math::Vector2f::zero()))
    );

    if (hovered)
    {
        m_to_menu_button->get_sprite().set_color(math::Vector4f({ 0.0f, 0.0f, 0.0f, 1.0f }));
        if (m_mouse.was_button_pressed(GLFW_MOUSE_BUTTON_1))
        {
            m_game.pop_state();
            return;
        }
    }
    else
    {
        m_to_menu_button->get_sprite().set_color(math::Vector4f::one());
    }
}

void AftermatchState::render()
{
    m_batch_renderer.begin();
    m_batch_renderer.submit(m_to_menu_button->get_sprite());
    m_batch_renderer.end();
    m_batch_renderer.flush();

    m_font_renderer.render_shadowed_text(*m_result_text_font, m_result_text,
                                         m_result_text_position,
                                         math::Vector2f({ 1.0f, -1.0f }),
                                         math::Vector4f::one(),
                                         math::Vector4f({ 0.0f, 0.0f, 0.0f, 1.0f }));
    m_to_menu_button->render_text(m_font_renderer);
}

} // namespace logic
