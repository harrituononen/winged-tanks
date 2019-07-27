#ifndef UTILITIES_SPAWN_POINT_HPP
#define UTILITIES_SPAWN_POINT_HPP

#include <map>

#include "../math/matrix.hpp"
#include "../graphics/texture.hpp"

namespace utilities {

struct SpawnPoint final
{
    math::Vector2f position;
    float          rotation;

     SpawnPoint() = default;
     SpawnPoint(math::Vector2f const& pos, float const rot);
    ~SpawnPoint() = default;
};

int constexpr NUM_SPAWN_POINTS = 8;
extern std::map<graphics::Textures, SpawnPoint[NUM_SPAWN_POINTS]> SPAWN_POINTS;

void init_spawn_points();

} // namespace utilities

#endif // UTILITIES_SPAWN_POINT_HPP
