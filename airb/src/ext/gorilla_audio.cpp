#include "gorilla_audio.hpp"

#include <stdexcept>

#include <gorilla/ga.h>

#include "../utilities/debug.hpp"

namespace ext {

GorillaAudio::GorillaAudio()
{
    if (gc_initialize(nullptr) != GC_SUCCESS)
        throw std::runtime_error("Gorilla Audio initialization failed.");

    utilities::Debug::log("Gorilla Audio initialized.");
}

GorillaAudio::~GorillaAudio()
{
    gc_shutdown();

    utilities::Debug::log("Gorilla Audio destroyed.");
}

} // namespace ext
