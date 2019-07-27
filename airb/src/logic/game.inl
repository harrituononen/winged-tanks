namespace logic {

inline graphics::Window& Game::get_window()
{
    return m_window;
}

inline graphics::BatchRenderer& Game::get_batch_renderer()
{
    return m_batch_renderer;
}

inline ui::FontRenderer& Game::get_font_renderer()
{
    return m_font_renderer;
}

inline ext::FreeType const& Game::get_freetype() const
{
    return m_freetype;
}

inline audio::Manager& Game::get_audio_manager()
{
    return m_audio_manager;
}

inline auto& Game::get_texture_holder()
{
    return m_texture_holder;
}

inline auto& Game::get_sound_holder()
{
    return m_sound_holder;
}

inline auto& Game::get_music_holder()
{
    return m_music_holder;
}

inline auto& Game::get_font_holder()
{
    return m_font_holder;
}

} // namespace logic
