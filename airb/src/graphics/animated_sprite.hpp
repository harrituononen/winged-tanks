#ifndef GRAPHICS_ANIMATED_SPRITE_HPP
#define GRAPHICS_ANIMATED_SPRITE_HPP

#include <chrono>

#include "sprite.hpp"
#include "../utilities/pool_object.hpp"

namespace graphics {

class AnimatedSprite : public Sprite, public utilities::PoolObject
{
protected:
    math::Vector2i            m_delta_position;
    std::chrono::milliseconds m_frame_time;
    int                       m_num_frames;
    std::chrono::milliseconds m_elapsed_time;
    int                       m_current_frame;

public:
     AnimatedSprite(Texture& tex,
                    math::Vector2i const& src_pos, math::Vector2i const& src_dim,
                    math::Vector2i const& dst_pos, math::Vector2i const& dst_dim,
                    math::Vector2i const& delta_pos,
                    std::chrono::milliseconds const frame_t, int const n_frames,
                    math::Vector2f const& anchor = math::Vector2f::zero(),
                    math::Vector2f const& tiling = math::Vector2f::one(),
                    GLfloat const rot = 0.0f,
                    std::array<math::Vector4f, 4> const& colors =
                    {
                        math::Vector4f::one(),
                        math::Vector4f::one(),
                        math::Vector4f::one(),
                        math::Vector4f::one()
                    });

    virtual ~AnimatedSprite() = default;

    AnimatedSprite            (AnimatedSprite const&) = delete;
    AnimatedSprite& operator= (AnimatedSprite const&) = delete;

    virtual void update(std::chrono::milliseconds const dt);
};

} // namespace graphics

#endif // GRAPHICS_ANIMATED_SPRITE_HPP
