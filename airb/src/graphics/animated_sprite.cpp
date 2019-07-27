#include "animated_sprite.hpp"

namespace graphics {

AnimatedSprite::AnimatedSprite(Texture& tex,
                               math::Vector2i const& src_pos, math::Vector2i const& src_dim,
                               math::Vector2i const& dst_pos, math::Vector2i const& dst_dim,
                               math::Vector2i const& delta_pos,
                               std::chrono::milliseconds const frame_t, int const n_frames,
                               math::Vector2f const& anchor,
                               math::Vector2f const& tiling,
                               GLfloat const rot,
                               std::array<math::Vector4f, 4> const& colors)
    : Sprite (tex, src_pos, src_dim, dst_pos, dst_dim, anchor, tiling, rot, colors)
    , PoolObject()

    , m_delta_position (delta_pos)
    , m_frame_time     (frame_t)
    , m_num_frames     (n_frames)
    , m_elapsed_time   (0)
    , m_current_frame  (0)
{ }

void AnimatedSprite::update(std::chrono::milliseconds const dt)
{
    m_elapsed_time += dt;

    if (m_elapsed_time >= m_frame_time)
    {
        m_elapsed_time -= m_frame_time;
        ++m_current_frame;

        if (m_current_frame < m_num_frames)
        {
            set_source_position(m_source_position + m_delta_position);
        }
        else
        {
            set_source_position(m_source_position - m_delta_position * (m_num_frames - 1));
            m_current_frame = 0;
        }
    }
}

} // namespace graphics
