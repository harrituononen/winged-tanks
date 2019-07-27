#include "manager.hpp"

#include <stdexcept>

namespace audio {

Manager::Manager()
    : m_gau_manager       (gau_manager_create())
    , m_ga_mixer          (nullptr)
    , m_ga_stream_manager (nullptr)
{
    if (m_gau_manager == nullptr)
        throw std::runtime_error("gau_Manager creation failed.");

    m_ga_mixer = gau_manager_mixer(m_gau_manager);
    if (m_ga_mixer == nullptr)
    {
        gau_manager_destroy(m_gau_manager);
        throw std::runtime_error("ga_Mixer retrieval failed.");
    }

    m_ga_stream_manager = gau_manager_streamManager(m_gau_manager);
    if (m_ga_stream_manager == nullptr)
    {
        gau_manager_destroy(m_gau_manager);
        throw std::runtime_error("ga_StreamManager retrieval failed.");
    }
}

Manager::~Manager()
{
    gau_manager_destroy(m_gau_manager);
}

} // namespace audio
