#ifndef NETWORK_UTILITIES_FUNCTIONS_HPP
#define NETWORK_UTILITIES_FUNCTIONS_HPP

#pragma warning(disable : 4996)

#include <ctime>
#include <iomanip>
#include <iostream>

namespace network {

inline void print_time()
{
    auto t = time(nullptr);
    struct tm* now = localtime(&t);
    std::cout << "[";
    std::cout << std::setw(2) << std::setfill('0') << now->tm_hour << ":";
    std::cout << std::setw(2) << std::setfill('0') << now->tm_min  << ":";
    std::cout << std::setw(2) << std::setfill('0') << now->tm_sec  << "] ";
}

} // namespace network

#endif // NETWORK_UTILITIES_FUNCTIONS_HPP
