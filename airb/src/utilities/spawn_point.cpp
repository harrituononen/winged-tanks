#include "spawn_point.hpp"

namespace utilities {

std::map<graphics::Textures, SpawnPoint[NUM_SPAWN_POINTS]> SPAWN_POINTS;

SpawnPoint::SpawnPoint(math::Vector2f const& pos, float const rot)
    : position (pos)
    , rotation (rot)
{ }

void init_spawn_points()
{
    using sp = SpawnPoint;

    SPAWN_POINTS[graphics::Textures::CAVE][0] = sp(math::Vector2f({  662.0f,  792.0f }),   0.0f);
    SPAWN_POINTS[graphics::Textures::CAVE][1] = sp(math::Vector2f({  527.0f, 2128.0f }),   0.0f);
    SPAWN_POINTS[graphics::Textures::CAVE][2] = sp(math::Vector2f({  509.0f, 3318.0f }),   0.0f);
    SPAWN_POINTS[graphics::Textures::CAVE][3] = sp(math::Vector2f({ 3050.0f, 1420.0f }),  90.0f);
    SPAWN_POINTS[graphics::Textures::CAVE][4] = sp(math::Vector2f({ 3053.0f, 2710.0f }), 270.0f);
    SPAWN_POINTS[graphics::Textures::CAVE][5] = sp(math::Vector2f({ 5464.0f,  816.0f }), 180.0f);
    SPAWN_POINTS[graphics::Textures::CAVE][6] = sp(math::Vector2f({ 5411.0f, 2315.0f }), 180.0f);
    SPAWN_POINTS[graphics::Textures::CAVE][7] = sp(math::Vector2f({ 5366.0f, 3421.0f }), 180.0f);

    SPAWN_POINTS[graphics::Textures::CITY][0] = sp(math::Vector2f({  478.0f,  357.0f }),   0.0f);
    SPAWN_POINTS[graphics::Textures::CITY][1] = sp(math::Vector2f({  504.0f, 2082.0f }),   0.0f);
    SPAWN_POINTS[graphics::Textures::CITY][2] = sp(math::Vector2f({  454.0f, 3625.0f }),   0.0f);
    SPAWN_POINTS[graphics::Textures::CITY][3] = sp(math::Vector2f({ 2881.0f,  945.0f }),  90.0f);
    SPAWN_POINTS[graphics::Textures::CITY][4] = sp(math::Vector2f({ 3109.0f, 3656.0f }), 270.0f);
    SPAWN_POINTS[graphics::Textures::CITY][5] = sp(math::Vector2f({ 5596.0f,  487.0f }), 180.0f);
    SPAWN_POINTS[graphics::Textures::CITY][6] = sp(math::Vector2f({ 5411.0f, 2154.0f }), 180.0f);
    SPAWN_POINTS[graphics::Textures::CITY][7] = sp(math::Vector2f({ 5537.0f, 3720.0f }), 180.0f);

    SPAWN_POINTS[graphics::Textures::DESERT][0] = sp(math::Vector2f({  208.0f, 1167.0f }),   0.0f);
    SPAWN_POINTS[graphics::Textures::DESERT][1] = sp(math::Vector2f({  240.0f, 2364.0f }),   0.0f);
    SPAWN_POINTS[graphics::Textures::DESERT][2] = sp(math::Vector2f({  223.0f, 3688.0f }),   0.0f);
    SPAWN_POINTS[graphics::Textures::DESERT][3] = sp(math::Vector2f({ 3064.0f, 2659.0f }),  90.0f);
    SPAWN_POINTS[graphics::Textures::DESERT][4] = sp(math::Vector2f({ 3007.0f, 3722.0f }), 270.0f);
    SPAWN_POINTS[graphics::Textures::DESERT][5] = sp(math::Vector2f({ 4272.0f, 2261.0f }), 180.0f);
    SPAWN_POINTS[graphics::Textures::DESERT][6] = sp(math::Vector2f({ 5651.0f, 2579.0f }), 180.0f);
    SPAWN_POINTS[graphics::Textures::DESERT][7] = sp(math::Vector2f({ 5668.0f, 3735.0f }), 180.0f);

    SPAWN_POINTS[graphics::Textures::JUNGLE][0] = sp(math::Vector2f({  292.0f,  357.0f }),   0.0f);
    SPAWN_POINTS[graphics::Textures::JUNGLE][1] = sp(math::Vector2f({  240.0f, 2364.0f }),   0.0f);
    SPAWN_POINTS[graphics::Textures::JUNGLE][2] = sp(math::Vector2f({  223.0f, 3688.0f }),   0.0f);
    SPAWN_POINTS[graphics::Textures::JUNGLE][3] = sp(math::Vector2f({ 2498.0f,  299.0f }),  90.0f);
    SPAWN_POINTS[graphics::Textures::JUNGLE][4] = sp(math::Vector2f({ 2275.0f, 3618.0f }), 270.0f);
    SPAWN_POINTS[graphics::Textures::JUNGLE][5] = sp(math::Vector2f({ 5363.0f,  705.0f }), 180.0f);
    SPAWN_POINTS[graphics::Textures::JUNGLE][6] = sp(math::Vector2f({ 5451.0f, 1938.0f }), 180.0f);
    SPAWN_POINTS[graphics::Textures::JUNGLE][7] = sp(math::Vector2f({ 5525.0f, 3835.0f }), 180.0f);

    SPAWN_POINTS[graphics::Textures::SNOW][0] = sp(math::Vector2f({  612.0f,  698.0f }),   0.0f);
    SPAWN_POINTS[graphics::Textures::SNOW][1] = sp(math::Vector2f({  683.0f, 2010.0f }),   0.0f);
    SPAWN_POINTS[graphics::Textures::SNOW][2] = sp(math::Vector2f({  922.0f, 3206.0f }),   0.0f);
    SPAWN_POINTS[graphics::Textures::SNOW][3] = sp(math::Vector2f({ 2738.0f,  748.0f }),  90.0f);
    SPAWN_POINTS[graphics::Textures::SNOW][4] = sp(math::Vector2f({ 2647.0f, 3294.0f }), 270.0f);
    SPAWN_POINTS[graphics::Textures::SNOW][5] = sp(math::Vector2f({ 5408.0f,  848.0f }), 180.0f);
    SPAWN_POINTS[graphics::Textures::SNOW][6] = sp(math::Vector2f({ 5180.0f, 1952.0f }), 180.0f);
    SPAWN_POINTS[graphics::Textures::SNOW][7] = sp(math::Vector2f({ 5298.0f, 3398.0f }), 180.0f);
}

} // namespace utilities
