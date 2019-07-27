#include "sound.hpp"

#include <stdexcept>
#include <stdlib.h>

#include "manager.hpp"
#include "../utilities/debug.hpp"

namespace audio {

Sound::Sound(Manager const& mgr, std::string const& path)
    : m_path       (path)
    , m_ga_mixer   (mgr.get_ga_mixer())
    , m_ga_sound   (gau_load_sound_file(path.c_str(), "wav"))
    , m_ga_handles ()
{
    if (m_ga_sound == nullptr)
        throw std::runtime_error("Sound (" + path + ") loading failed.");

    m_ga_handles.reserve(128);

    utilities::Debug::log("Sound " + m_path + " loaded.");
}

Sound::~Sound()
{
    for (auto hndl_ptr : m_ga_handles)
        ga_handle_destroy(hndl_ptr);
    ga_sound_release(m_ga_sound);

    utilities::Debug::log("Sound " + m_path + " destroyed.");
}

ga_Handle* Sound::play() 
{
    size_t const len = m_ga_handles.size();
    for (size_t i = 0; i < len; ++i)
    {
        size_t const hndl_idx = len - 1 - i;
        ga_Handle* const hndl = m_ga_handles[hndl_idx];
        if (ga_handle_finished(hndl))
        {
            ga_handle_destroy(hndl);
            m_ga_handles.erase(m_ga_handles.begin() + hndl_idx);
        }
    }

    ga_Handle* const hndl = gau_create_handle_sound(m_ga_mixer, m_ga_sound,
                                                    nullptr, nullptr, nullptr);
    if (hndl == nullptr)
        throw std::runtime_error("ga_Handle creation failed.");

    m_ga_handles.push_back(hndl);
    ga_handle_play(hndl);
    return hndl;
}

} // namespace audio
