#ifdef DEBUG
    #include <iostream>
#endif

namespace utilities {

inline void Debug::log(std::string const& msg)
{
#ifdef DEBUG
    std::cout << "-- DEBUG -- " << msg << std::endl;
#else
    (void) msg;
#endif
}

} // namespace utilities
