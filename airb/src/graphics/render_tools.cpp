#include "render_tools.hpp"

#include "shader.hpp"
#include "window.hpp"
#include "../logic/game.hpp"

namespace graphics {

void RenderTools::set_projection_dimensions(Shader& sh, math::Vector2f const& dim)
{
    sh.set_matrix_uniform_4fv(
        "projection",
        math::Matrix4f::ortho(dim[X], 0.0f, dim[Y], 0.0f, 1.0f, -1.0f)
    );
}

void RenderTools::set_transform(Shader& sh, math::Vector2f const& t)
{
    sh.set_matrix_uniform_3fv("transform", math::Matrix3f::translate(-t));
}

void RenderTools::set_scale(Shader& sh, math::Vector2f const& s)
{
    sh.set_matrix_uniform_3fv("scale", math::Matrix3f::scale(s));
}

void RenderTools::scale_to_game_dimensions(Shader& sh, Window const& win)
{
    float const s = win.get_dimensions()[X] / static_cast<float>(logic::Game::TARGET_DIMENSIONS[X]);
    graphics::RenderTools::set_scale(sh, math::Vector2f({ s, s }));
}

void RenderTools::enable_ui_mode(Shader& sh, Window const& win)
{
    graphics::RenderTools::set_projection_dimensions(sh, math::Vector2f(win.get_dimensions()));
    graphics::RenderTools::set_transform(sh, math::Vector2f::zero());
    graphics::RenderTools::set_scale(sh, math::Vector2f::one());
}

void RenderTools::enable_world_mode(Shader& sh, Window const& win)
{
    graphics::RenderTools::set_projection_dimensions(sh, math::Vector2f(win.get_dimensions()));
    graphics::RenderTools::set_transform(sh, math::Vector2f::zero());
    graphics::RenderTools::scale_to_game_dimensions(sh, win);
}

} // namespace graphics
