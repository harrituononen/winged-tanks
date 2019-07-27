#include "explosion.hpp"

namespace graphics
{

Explosion::Explosion(Texture& tex,
                     math::Vector2i const& src_pos, math::Vector2i const& src_dim,
                     math::Vector2i const& dst_pos, math::Vector2i const& dst_dim,
                     math::Vector2i const& delta_pos,
                     std::chrono::milliseconds const frame_t, int const n_frames,
                     math::Vector2f const& anchor)
    : AnimatedSprite(tex, src_pos, src_dim, dst_pos, dst_dim,
                     delta_pos, frame_t, n_frames, anchor)
{ }

void Explosion::update(std::chrono::milliseconds const dt)
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
            m_elapsed_time = std::chrono::milliseconds::zero();
            m_current_frame = 0;
            deactivate();
        }
    }
}

} // namespace graphics
