namespace audio {

inline void Sound::set_gain(ga_Handle* const hndl, gc_float32 const g) const
{
    ga_handle_setParamf(hndl, GA_HANDLE_PARAM_GAIN, g);
}

inline void Sound::set_pitch(ga_Handle* const hndl, gc_float32 const p) const
{
    ga_handle_setParamf(hndl, GA_HANDLE_PARAM_PITCH, p);
}

inline void Sound::set_pan(ga_Handle* const hndl, gc_float32 const p) const
{
    ga_handle_setParamf(hndl, GA_HANDLE_PARAM_PAN, p);
}

inline ga_Sound* Sound::get_ga_sound() const
{
    return m_ga_sound;
}

} // namespace audio
