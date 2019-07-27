#ifndef GRAPHICS_EXPLOSION_HPP
#define GRAPHICS_EXPLOSION_HPP

#include "animated_sprite.hpp"

namespace graphics
{

class Explosion final : public AnimatedSprite
{
public:
     Explosion(Texture& tex,
               math::Vector2i const& src_pos, math::Vector2i const& src_dim,
               math::Vector2i const& dst_pos, math::Vector2i const& dst_dim,
               math::Vector2i const& delta_pos,
               std::chrono::milliseconds const frame_t, int const n_frames,
               math::Vector2f const& anchor);
    ~Explosion() = default;

    Explosion            (Explosion const&) = delete;
    Explosion& operator= (Explosion const&) = delete;

    void update(std::chrono::milliseconds const dt) override;
};

} // namespace graphics

#endif // GRAPHICS_EXPLOSION_HPP
