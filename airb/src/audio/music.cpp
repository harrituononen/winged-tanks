#include "music.hpp"

#include <stdexcept>

#include "manager.hpp"
#include "../utilities/debug.hpp"

namespace audio {

Music::Music(Manager const& mgr, std::string const& path)
    : m_path      (path)
    , m_loop      ()
    , m_ga_handle (gau_create_handle_buffered_file(mgr.get_ga_mixer(), mgr.get_ga_stream_manager(),
                                                   path.c_str(), "ogg",
                                                   nullptr, nullptr, &m_loop))
{
    if (m_ga_handle == nullptr)
        throw std::runtime_error("Music (" + path + ") loading failed.");

    gau_sample_source_loop_set(m_loop, -1, 0);

    utilities::Debug::log("Music " + m_path + " loaded.");
}

Music::~Music()
{
    ga_handle_destroy(m_ga_handle);

    utilities::Debug::log("Music " + m_path + " destroyed.");
}

void Music::play()
{
    ga_handle_play(m_ga_handle);
}

void Music::stop()
{
    ga_handle_stop(m_ga_handle);
}

} // namespace audio
