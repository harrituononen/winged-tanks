#include "file_io.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace utilities {

std::string FileIO::get_string_from_file(std::string const& path)
{
    std::ifstream f(path);
    if (!f.is_open())
        throw std::runtime_error("Opening file " + path + " failed.");
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

} // namespace utilities
