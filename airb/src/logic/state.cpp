#include "state.hpp"

#include "game.hpp"
#include "../graphics/batch_renderer.hpp"
#include "../graphics/window.hpp"
#include "../utilities/resource_holder.hpp"

namespace logic {

State::State(Game& g)
    : m_game(g)

    , m_window         (g.get_window())
    , m_batch_renderer (g.get_batch_renderer())
    , m_font_renderer  (g.get_font_renderer())

    , m_texture_holder (g.get_texture_holder())
    , m_sound_holder   (g.get_sound_holder())
    , m_music_holder   (g.get_music_holder())
    , m_font_holder    (g.get_font_holder())

    , m_audio_manager (g.get_audio_manager())

    , m_keyboard (m_window.keyboard)
    , m_mouse    (m_window.mouse)
{ }

} // namespace logic
