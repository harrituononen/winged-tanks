#ifndef LOGIC_GAME_HPP
#define LOGIC_GAME_HPP

#include <chrono>
#include <memory>
#include <stack>
#include <string>
#include <vector>

#include "state.hpp"
#include "../audio/manager.hpp"
#include "../audio/music.hpp"
#include "../audio/sound.hpp"
#include "../ext/freetype.hpp"
#include "../ext/glew.hpp"
#include "../ext/glfw.hpp"
#include "../ext/gorilla_audio.hpp"
#include "../graphics/batch_renderer.hpp"
#include "../graphics/texture.hpp"
#include "../graphics/window.hpp"
#include "../ui/font_renderer.hpp"
#include "../math/matrix.hpp"
#include "../network/connection.hpp"
#include "../network/socket/client_socket_win.hpp"
#include "../network/socket/server_socket_win.hpp"
#include "../utilities/font_holder.hpp"
#include "../utilities/resource_holder.hpp"

namespace logic {

class Game final
{
public:
    static math::Vector2i const TARGET_DIMENSIONS;

private:
    ext::GLFW const         m_glfw;
    graphics::Window        m_window;
    ext::GLEW const         m_glew;
    ext::FreeType const     m_freetype;
    ext::GorillaAudio const m_gorilla_audio;
    audio::Manager          m_audio_manager;

    graphics::BatchRenderer m_batch_renderer;
    ui::FontRenderer        m_font_renderer;

    utilities::ResourceHolder<graphics::Texture, graphics::Textures> m_texture_holder;
    utilities::ResourceHolder<audio::Sound, audio::Sounds>           m_sound_holder;
    utilities::ResourceHolder<audio::Music, audio::MusicThemes>      m_music_holder;
    utilities::FontHolder                                            m_font_holder;

    std::stack<std::unique_ptr<State>> m_states;

    bool m_running;
    bool m_show_fps;

    std::chrono::time_point<std::chrono::steady_clock> m_current_time;
    std::chrono::time_point<std::chrono::steady_clock> m_previous_time;

public:
     Game();
    ~Game() = default;

    Game            (Game const&) = delete;
    Game& operator= (Game const&) = delete;

    void run  ();
    void stop ();

    void load_level          (graphics::Textures const lvl_id, std::string const& lvl_tex_path);
    void load_level_with_pop (graphics::Textures const lvl_id, std::string const& lvl_tex_path);
    void unload_level        (graphics::Textures const lvl_id);

    void start_single_player_game(graphics::Textures const lvl_id,
                                  std::string const& lvl_tex_path);
    void host_game(graphics::Textures const lvl_id,
                   std::string const& lvl_tex_path,
                   std::unique_ptr<network::ServerSocket> socket,
                   std::vector<std::shared_ptr<network::Connection>> players);
    void join_game(graphics::Textures const lvl_id,
                   std::string const& lvl_tex_path,
                   std::unique_ptr<network::ClientSocket> socket,
                   unsigned char const public_id,
                   std::vector<std::shared_ptr<network::Connection>> players);

    void reset_graphics ();
    void reset_time     ();

private:
    void update_fps();

public:
    void push_state (std::unique_ptr<State> state);
    void pop_state  ();

    inline graphics::Window&        get_window         ();
    inline graphics::BatchRenderer& get_batch_renderer ();
    inline ui::FontRenderer& get_font_renderer  ();

    inline ext::FreeType const& get_freetype() const;

    inline audio::Manager& get_audio_manager();

    auto& get_texture_holder ();
    auto& get_sound_holder   ();
    auto& get_music_holder   ();
    auto& get_font_holder    ();
};

} // namespace logic

#include "game.inl"

#endif // LOGIC_GAME_HPP
