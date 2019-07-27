#ifndef AUDIO_SOUND_HPP
#define AUDIO_SOUND_HPP

#include <string>
#include <vector>

#include <gorilla/ga.h>

namespace audio {

enum class Sounds
{
    CANNON_FIRE,
    SYRINGE_FIRE,
    NEEDLER_FIRE
};

class Manager;

class Sound final
{
private:
    std::string const m_path;
    ga_Mixer*         m_ga_mixer;
    ga_Sound*         m_ga_sound;

    std::vector<ga_Handle*> m_ga_handles;

public:
     Sound(Manager const& mgr, std::string const& path);
    ~Sound();

    Sound            (Sound const&) = delete;
    Sound& operator= (Sound const&) = delete;

    ga_Handle* play();

    inline void set_gain  (ga_Handle* const hndl, gc_float32 const g) const;
    inline void set_pitch (ga_Handle* const hndl, gc_float32 const p) const;
    inline void set_pan   (ga_Handle* const hndl, gc_float32 const p) const;

    inline ga_Sound* get_ga_sound() const;
};

} // namespace audio

#include "sound.inl"

#endif // AUDIO_SOUND_HPP
