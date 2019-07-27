#ifndef UTILITIES_DEBUG_HPP
#define UTILITIES_DEBUG_HPP

#include <string>

namespace utilities {

namespace Debug
{
    inline void log(std::string const& msg);
}

} // namespace utilities

#include "debug.inl"

#endif // UTILITIES_DEBUG_HPP
