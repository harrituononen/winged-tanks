#ifndef EXT_GLEW_HPP
#define EXT_GLEW_HPP

namespace ext {

class GLEW final
{
public:
     GLEW();
    ~GLEW();

    GLEW            (GLEW const&) = delete;
    GLEW& operator= (GLEW const&) = delete;
};

} // namespace ext

#endif // EXT_GLEW_HPP
