#include "projectile.hpp"
#include "../logic/gameplay_state.hpp"

namespace entities {

int const Projectile::EXPLOSION_RADII[] = { 50, 30, 20 };

namespace
{
    math::Vector2i const PROJECTILE_TEXTURE_POSITION({ graphics::PROJECTILE_OFFSET_X,
                                                       graphics::PROJECTILE_OFFSET_Y });
    math::Vector2i const PROJECTILE_TEXTURE_DIMENSIONS({ graphics::PROJECTILE_DIMENSIONS,
                                                         graphics::PROJECTILE_DIMENSIONS });
}

Projectile::Projectile(graphics::Texture& tex, logic::GameplayState* game_state)
    : PoolObject()

    , m_damage       (0)
    , m_type         (0)
    , m_lifetime     (std::chrono::milliseconds::zero())
    , m_elapsed_time (std::chrono::milliseconds::zero())
    , m_game_state   (game_state)

    , m_sprite   (tex, PROJECTILE_TEXTURE_POSITION, PROJECTILE_TEXTURE_DIMENSIONS,
                  math::Vector2i::zero(), PROJECTILE_TEXTURE_DIMENSIONS / 4,
                  math::Vector2f({ 0.5f, 0.5f }))
{ }

} // namespace entities
