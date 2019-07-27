#ifndef AUDIO_MUSIC_HPP
#define AUDIO_MUSIC_HPP

#include <string>

#include <gorilla/ga.h>
#include <gorilla/gau.h>

namespace audio {

enum class MusicThemes
{
    MAIN_MENU,
    LOBBY,
    STALACTICA_AND_URBAN_APEX,
    PRIMEVAL_WOODS,
    DESOLATE_DUNES,
    SNOW_CLAD_ADOBE
};

class Manager;

class Music final
{
private:
    std::string const     m_path;
    gau_SampleSourceLoop* m_loop;
    ga_Handle*            m_ga_handle;

public:
     Music(Manager const& mgr, std::string const& path);
    ~Music();

    Music            (Music const&) = delete;
    Music& operator= (Music const&) = delete;

    void play();
    void stop();
};

} // namespace audio

#endif // AUDIO_MUSIC_HPP
