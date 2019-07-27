#ifndef EXT_GLFW_HPP
#define EXT_GLFW_HPP

namespace ext {

class GLFW final
{
public:
     GLFW();
    ~GLFW();

    GLFW            (GLFW const&) = delete;
    GLFW& operator= (GLFW const&) = delete;
};

} // namespace ext

#endif // EXT_GLFW_HPP
