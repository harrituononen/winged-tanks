#ifndef LOGIC_STATE_HPP
#define LOGIC_STATE_HPP

#include <chrono>

#include "../audio/manager.hpp"
#include "../audio/music.hpp"
#include "../audio/sound.hpp"
#include "../graphics/texture.hpp"

namespace graphics {

class BatchRenderer;
class Window;

} // namespace graphics

namespace input {

class Keyboard;
class Mouse;

} // namespace input

namespace utilities {

template<typename T, typename Id> class ResourceHolder;
                                  class FontHolder;

} // namespace utilities

namespace ui {

class FontRenderer;

} // namespace ui

namespace logic {

class Game;

class State
{
protected:
    Game& m_game;

    graphics::Window&        m_window;
    graphics::BatchRenderer& m_batch_renderer;
    ui::FontRenderer&        m_font_renderer;

    utilities::ResourceHolder<graphics::Texture, graphics::Textures>& m_texture_holder;
    utilities::ResourceHolder<audio::Sound, audio::Sounds>&           m_sound_holder;
    utilities::ResourceHolder<audio::Music, audio::MusicThemes>&      m_music_holder;
    utilities::FontHolder&                                            m_font_holder;

    audio::Manager& m_audio_manager;

    input::Keyboard& m_keyboard;
    input::Mouse&    m_mouse;

public:
    explicit  State(Game& g);
    virtual  ~State() = default;

    State            (State const&) = delete;
    State& operator= (State const&) = delete;

    virtual void on_enter () = 0;
    virtual void on_exit  () = 0;

    virtual void update(std::chrono::milliseconds const dt) = 0;
    virtual void render() = 0;
};

} // namespace logic

#endif // LOGIC_STATE_HPP
