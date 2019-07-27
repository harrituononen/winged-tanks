#ifndef AUDIO_MANAGER_HPP
#define AUDIO_MANAGER_HPP

#include <gorilla/ga.h>
#include <gorilla/gau.h>

namespace audio {

class Manager final
{
private:
    gau_Manager*      m_gau_manager;
    ga_Mixer*         m_ga_mixer;
    ga_StreamManager* m_ga_stream_manager;

public:
     Manager();
    ~Manager();

    Manager            (Manager const&) = delete;
    Manager& operator= (Manager const&) = delete;

    inline void update() const;

    inline gau_Manager*      get_gau_manager       () const;
    inline ga_Mixer*         get_ga_mixer          () const;
    inline ga_StreamManager* get_ga_stream_manager () const;
};

} // namespace audio

#include "manager.inl"

#endif // AUDIO_MANAGER_HPP
