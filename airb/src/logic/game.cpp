#include "game.hpp"

#include <iostream>
#include <memory>
#include <thread>

#include <GLFW/glfw3.h>

#include "aftermatch_state.hpp"
#include "gameplay_state.hpp"
#include "client_gameplay_state.hpp"
#include "server_gameplay_state.hpp"
#include "title_state.hpp"
#include "lobby_state.hpp"
#include "../graphics/render_tools.hpp"
#include "../graphics/sprite.hpp"
#include "../ui/font.hpp"

namespace logic {

math::Vector2i const Game::TARGET_DIMENSIONS({ 1920, 1080 });

namespace
{
    auto const FRAME_RATE = 300;
    auto const FRAME_TIME = std::chrono::nanoseconds(1000000000L / FRAME_RATE);
}

Game::Game()
    : m_glfw          ()
    , m_window        ("Winged Tanks", 1280, 720)
    , m_glew          ()
    , m_freetype      ()
    , m_gorilla_audio ()
    , m_audio_manager ()

    , m_batch_renderer ()
    , m_font_renderer  ()

    , m_texture_holder ()
    , m_sound_holder   ()
    , m_music_holder   ()
    , m_font_holder    ()

    , m_states ()

    , m_running  (false)
    , m_show_fps (true)

    , m_current_time  ()
    , m_previous_time ()
{

    m_texture_holder.load(
        graphics::Textures::CROSSHAIR,
        std::make_unique<graphics::Texture>("res/textures/crosshair.tga")
    );

    m_texture_holder.load(
        graphics::Textures::GAMEPLAY_ELEMENTS,
        std::make_unique<graphics::Texture>("res/textures/gameplay_elements.tga")
    );

    m_texture_holder.load(
        graphics::Textures::UI,
        std::make_unique<graphics::Texture>("res/textures/ui.tga")
    );

    m_texture_holder.load(
        graphics::Textures::MENU_BACKGROUND,
        std::make_unique<graphics::Texture>("res/textures/menu_background.tga")
    );

    m_sound_holder.load(
        audio::Sounds::CANNON_FIRE,
        std::make_unique<audio::Sound>(m_audio_manager, "res/audio/sfx/cannon.wav")
    );

    m_sound_holder.load(
        audio::Sounds::SYRINGE_FIRE,
        std::make_unique<audio::Sound>(m_audio_manager, "res/audio/sfx/syringe.wav")
    );

    m_sound_holder.load(
        audio::Sounds::NEEDLER_FIRE,
        std::make_unique<audio::Sound>(m_audio_manager, "res/audio/sfx/needler.wav")
    );

    m_music_holder.load(
        audio::MusicThemes::MAIN_MENU,
        std::make_unique<audio::Music>(m_audio_manager, "res/audio/bgm/main_menu.ogg")
    );

    m_music_holder.load(
        audio::MusicThemes::LOBBY,
        std::make_unique<audio::Music>(m_audio_manager, "res/audio/bgm/lobby.ogg")
    );

    m_music_holder.load(
        audio::MusicThemes::STALACTICA_AND_URBAN_APEX,
        std::make_unique<audio::Music>(m_audio_manager, "res/audio/bgm/cave_and_city.ogg")
    );

    m_music_holder.load(
        audio::MusicThemes::PRIMEVAL_WOODS,
        std::make_unique<audio::Music>(m_audio_manager, "res/audio/bgm/jungle.ogg")
    );

    m_music_holder.load(
        audio::MusicThemes::DESOLATE_DUNES,
        std::make_unique<audio::Music>(m_audio_manager, "res/audio/bgm/desert.ogg")
    );

    m_music_holder.load(
        audio::MusicThemes::SNOW_CLAD_ADOBE,
        std::make_unique<audio::Music>(m_audio_manager, "res/audio/bgm/snow.ogg")
    );

    push_state(std::make_unique<TitleState>(*this));
}

void Game::run()
{
    m_previous_time = std::chrono::high_resolution_clock::now();
    m_running = true;

    while (m_running && m_window.is_open())
    {
        auto const start_t = std::chrono::high_resolution_clock::now();
        auto const end_t   = start_t + FRAME_TIME;

        // Reset input
        m_window.keyboard.reset_frame();
        m_window.mouse.reset_frame();

        // Poll events
        glfwPollEvents();

        // Update game state
        m_current_time = std::chrono::high_resolution_clock::now();
        auto const dt  = std::chrono::duration_cast<std::chrono::milliseconds>(
            m_current_time - m_previous_time
        );
        m_states.top()->update(dt);
        m_previous_time = m_current_time;

        // Update audio
        m_audio_manager.update();

        // Render game state
        graphics::RenderTools::clear_screen();
        m_states.top()->render();
        m_window.swap_buffers();

        // Limit frame rate
        std::this_thread::sleep_until(end_t);

        // Update FPS
        if (m_show_fps)
            update_fps();
    }
}

void Game::stop()
{
    m_running = false;
}

void Game::load_level(graphics::Textures const lvl_id, std::string const& lvl_tex_path)
{
    reset_graphics();

    static int const LOADING_FONT_SIZE = 64;

    m_font_holder.load(
        ui::Fonts::BASIC_SANS, LOADING_FONT_SIZE,
        std::make_unique<ui::Font>(m_freetype, "res/fonts/riffic_free.ttf",
                                   LOADING_FONT_SIZE)
    );

    m_font_renderer.render_shadowed_text(
        m_font_holder.get(ui::Fonts::BASIC_SANS, LOADING_FONT_SIZE),
        "Loading...",
        math::Vector2f::one() * 20.0f,
        math::Vector2f({ 1.0f, -1.0f })
    );

    m_window.swap_buffers();

    m_font_holder.destroy(ui::Fonts::BASIC_SANS, LOADING_FONT_SIZE);

    m_texture_holder.load(lvl_id, std::make_unique<graphics::Texture>(lvl_tex_path, false));
}

void Game::load_level_with_pop(graphics::Textures const lvl_id, std::string const& lvl_tex_path)
{
    pop_state();
    load_level(lvl_id, lvl_tex_path);
}

void Game::unload_level(graphics::Textures const lvl_id)
{
    m_texture_holder.destroy(lvl_id);
}

void Game::start_single_player_game(graphics::Textures const lvl_id,
                                    std::string const& lvl_tex_path)
{
    load_level_with_pop(lvl_id, lvl_tex_path);
    push_state(std::make_unique<GameplayState>(*this, lvl_id));
}

void Game::host_game(graphics::Textures const lvl_id,
                     std::string const& lvl_tex_path,
                     std::unique_ptr<network::ServerSocket> socket,
                     std::vector<std::shared_ptr<network::Connection>> players)
{
    load_level_with_pop(lvl_id, lvl_tex_path);
    push_state(std::make_unique<ServerGameplayState>(*this, lvl_id, std::move(socket), players));
}

void Game::join_game(graphics::Textures const lvl_id,
                     std::string const& lvl_tex_path,
                     std::unique_ptr<network::ClientSocket> socket,
                     unsigned char const public_id,
                     std::vector<std::shared_ptr<network::Connection>> players)
{
    load_level_with_pop(lvl_id, lvl_tex_path);
    push_state(std::make_unique<ClientGameplayState>(*this, lvl_id, std::move(socket), public_id, players));
}

void Game::reset_graphics()
{
    m_window.set_cursor_visible(true);

    graphics::RenderTools::enable_world_mode(m_font_renderer.get_shader(), m_window);
    graphics::RenderTools::enable_world_mode(m_batch_renderer.get_shader(), m_window);

    graphics::RenderTools::set_clear_color(0.0f, 0.0f, 0.0f, 1.0f);
    graphics::RenderTools::clear_screen();
}

void Game::reset_time()
{
    m_previous_time = std::chrono::high_resolution_clock::now();
    m_current_time  = m_previous_time;
}

void Game::update_fps()
{
    static auto fps_t      = std::chrono::high_resolution_clock::now();
    static auto next_fps_t = fps_t + std::chrono::seconds(1);
    static int  num_frames = 0;

    auto const curr_t = std::chrono::high_resolution_clock::now();
    fps_t += std::chrono::duration_cast<std::chrono::nanoseconds>(curr_t - fps_t);

    if (fps_t >= next_fps_t)
    {
        next_fps_t += std::chrono::seconds(1);
        m_window.set_title("Winged Tanks, FPS: " + std::to_string(num_frames));
        num_frames = 0;
    }

    ++num_frames;
}

void Game::push_state(std::unique_ptr<State> state)
{
    if (!m_states.empty())
        m_states.top()->on_exit();
    m_states.emplace(std::move(state));
    reset_graphics();
    m_states.top()->on_enter();

    reset_time();
}

void Game::pop_state()
{
    m_states.top()->on_exit();
    m_states.pop();
    if (!m_states.empty())
    {
        reset_graphics();
        m_states.top()->on_enter();
        reset_time();
    }
}

} // namespace logic
