#include "../utilities/debug.hpp"

namespace graphics {

inline void RenderTools::set_clear_color(GLfloat const r,
                                         GLfloat const g,
                                         GLfloat const b,
                                         GLfloat const a)
{
    glClearColor(r, g, b, a);

    utilities::Debug::log(
        "Clear color set to ("
      + std::to_string(r) + ", " + std::to_string(g) + ", "
      + std::to_string(g) + ", " + std::to_string(a) + ")."
    );
}

inline void RenderTools::clear_screen()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

inline void RenderTools::unuse_shader()
{
    glUseProgram(0);
}

inline void RenderTools::unbind_texture()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

inline void RenderTools::enable_transparency()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    utilities::Debug::log("Transparency enabled.");
}

inline void RenderTools::enable_textures()
{
    glEnable(GL_TEXTURE_2D);

    utilities::Debug::log("Textures enabled.");
}

} // namespace graphics
