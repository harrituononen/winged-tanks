#ifndef GRAPHICS_RENDER_TOOLS_HPP
#define GRAPHICS_RENDER_TOOLS_HPP

#include <string>

#include <glew.h>

#include "../math/matrix.hpp"

namespace graphics {

class Shader;
class Window;

namespace RenderTools
{
    inline void set_clear_color (GLfloat const r,
                                 GLfloat const g,
                                 GLfloat const b,
                                 GLfloat const a);
    inline void clear_screen    ();

    inline void unuse_shader   ();
    inline void unbind_texture ();

    void set_projection_dimensions (Shader& sh, math::Vector2f const& dim);
    void set_transform             (Shader& sh, math::Vector2f const& t);
    void set_scale                 (Shader& sh, math::Vector2f const& s);
    void scale_to_game_dimensions  (Shader& sh, Window const& win);
    void enable_ui_mode            (Shader& sh, Window const& win);
    void enable_world_mode         (Shader& sh, Window const& win);

    inline void enable_textures     ();
    inline void enable_transparency ();
}

} // namespace graphics

#include "render_tools.inl"

#endif // GRAPHICS_RENDER_TOOLS_HPP
