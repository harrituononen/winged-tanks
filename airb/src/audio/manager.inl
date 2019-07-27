namespace audio {

inline void Manager::update() const
{
    gau_manager_update(m_gau_manager);
}

inline gau_Manager* Manager::get_gau_manager() const
{
    return m_gau_manager;
}

inline ga_Mixer* Manager::get_ga_mixer() const
{
    return m_ga_mixer;
}

inline ga_StreamManager* Manager::get_ga_stream_manager() const
{
    return m_ga_stream_manager;
}

} // namespace audio
