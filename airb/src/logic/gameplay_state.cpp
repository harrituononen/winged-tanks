#include "gameplay_state.hpp"

#include <algorithm>
#include <cstdlib>

#include <GLFW/glfw3.h>

#include "game.hpp"
#include "../graphics/render_tools.hpp"
#include "../graphics/texture.hpp"
#include "../ui/font.hpp"
#include "../math/general.hpp"
#include "../physics/collision.hpp"

namespace logic {

namespace
{
    int const HUD_FONT_SIZE = 48;

    int const SCOREBOARD_TITLE_FONT_SIZE = 96;
    int const SCOREBOARD_SCORE_FONT_SIZE = 36;

    math::Vector2i const PLANE_TEXTURE_POSITION({ graphics::PLANE_OFFSET_X,
                                                  graphics::PLANE_OFFSET_Y });
    math::Vector2i const PLANE_TEXTURE_DIMENSIONS({ graphics::PLANE_DIMENSIONS,
                                                    graphics::PLANE_DIMENSIONS });
    math::Vector2i const EXPLOSION_TEXTURE_POSITION({ graphics::EXPLOSION_OFFSET_X,
                                                      graphics::EXPLOSION_OFFSET_Y });
    math::Vector2i const EXPLOSION_TEXTURE_DIMENSIONS({ graphics::EXPLOSION_DIMENSIONS,
                                                        graphics::EXPLOSION_DIMENSIONS });
}

GameplayState::GameplayState(Game& g, graphics::Textures const lvl_id)
    : State (g)

    , m_level_id (lvl_id)

    , m_gameplay_elements_texture (m_texture_holder.get(graphics::Textures::GAMEPLAY_ELEMENTS))
    , m_level_texture             (m_texture_holder.get(lvl_id))
    , m_crosshair_texture         (m_texture_holder.get(graphics::Textures::CROSSHAIR))
    , m_player                    (1.0f, 50000.0f, 1.75f, 
                                   std::vector<physics::BoxCollider>
                                  {physics::BoxCollider(std::vector<math::Vector2f>
                                  {math::Vector2f({ 879.0f, 991.0f }),
                                   math::Vector2f({ 905.0f, 991.0f }),
                                   math::Vector2f({ 905.0f, 812.0f }),
                                   math::Vector2f({ 879.0f, 812.0f })}),
                                   physics::BoxCollider(std::vector<math::Vector2f>
                                  {math::Vector2f({ 819.0f, 916.0f }),
                                   math::Vector2f({ 916.0f, 916.0f }),
                                   math::Vector2f({ 916.0f, 879.0f }),
                                   math::Vector2f({ 819.0f, 879.0f })})}, 
                                   1000.0f, 40.0f,
                                   m_texture_holder,
                                   this,
                                   m_gameplay_elements_texture,
                                   PLANE_TEXTURE_POSITION, PLANE_TEXTURE_DIMENSIONS,
                                   math::Vector2i::one() * Game::TARGET_DIMENSIONS[X] / 10,
                                   math::Vector2f::one() * 900.0f) //spawn_point
    , m_ai(1.0f, 50000.0f, 1.75f,
                                   std::vector<physics::BoxCollider>
                                   {physics::BoxCollider(std::vector<math::Vector2f>
                                   {math::Vector2f({ 879.0f, 991.0f }),
                                   math::Vector2f({ 905.0f, 991.0f }),
                                   math::Vector2f({ 905.0f, 812.0f }),
                                   math::Vector2f({ 879.0f, 812.0f })}),
                                   physics::BoxCollider(std::vector<math::Vector2f>
                                   {math::Vector2f({ 819.0f, 916.0f }),
                                   math::Vector2f({ 916.0f, 916.0f }),
                                   math::Vector2f({ 916.0f, 879.0f }),
                                   math::Vector2f({ 819.0f, 879.0f })})},
                                   1000.0f, 40.0f,
                                   m_texture_holder,
                                   this,
                                   m_gameplay_elements_texture,
                                   PLANE_TEXTURE_POSITION, PLANE_TEXTURE_DIMENSIONS,
                                   math::Vector2i::one() * Game::TARGET_DIMENSIONS[X] / 10,
                                   math::Vector2f::one() * 900.0f, //spawn_point
                                   m_terrain)
    , m_terrain                   (m_level_texture)

    , m_terrain_dimensions (math::Vector2i({ m_level_texture.get_dimensions()[X],
                                             m_level_texture.get_dimensions()[Y] / 2 }))
    , m_terrain_sprite     (m_level_texture,
                            math::Vector2i::zero(), m_terrain_dimensions,
                            math::Vector2i::zero(), m_terrain_dimensions)
    , m_background_sprite  (m_level_texture,
                            math::Vector2i::up() * m_terrain_dimensions[Y], m_terrain_dimensions,
                            math::Vector2i::zero(), m_terrain_dimensions)
    , m_crosshair_sprite   (m_crosshair_texture,
                            math::Vector2i::zero(),
                            math::Vector2i::one() * m_crosshair_texture.get_dimensions()[X],
                            math::Vector2i::zero(),
                            math::Vector2i::one()
                            * m_crosshair_texture.get_dimensions()[X]
                            * Game::TARGET_DIMENSIONS[X] / m_window.get_dimensions()[X],
                            math::Vector2f::one() * 0.5f)

    , m_camera_position      (math::Vector2f::zero())
    , m_mouse_world_position (math::Vector2f::zero())

    , m_hud_font           (nullptr)
    , m_kill_notifications (nullptr)

    , m_scoreboard           (nullptr)
    , m_show_scoreboard      (false)

    , m_previous_ammo_count (-1)
    , m_ammo_text           ("")
    , m_ammo_text_position  ()

    , m_previous_health      (-1)
    , m_health_text          ("")
    , m_health_text_position (math::Vector2f({ 20.0f, 20.0f }))

    , m_current_weapon_index         (-1)
    , m_current_weapon_text          ("")
    , m_current_weapon_text_position ()

    , m_fire_sounds () 
    #define THEME(x) (m_music_holder.get(x))
    , m_music(
        lvl_id == graphics::Textures::CAVE   ? THEME(audio::MusicThemes::STALACTICA_AND_URBAN_APEX)
      : lvl_id == graphics::Textures::CITY   ? THEME(audio::MusicThemes::STALACTICA_AND_URBAN_APEX)
      : lvl_id == graphics::Textures::JUNGLE ? THEME(audio::MusicThemes::PRIMEVAL_WOODS)
      : lvl_id == graphics::Textures::DESERT ? THEME(audio::MusicThemes::DESOLATE_DUNES)
      :                                        THEME(audio::MusicThemes::SNOW_CLAD_ADOBE)
    )
    #undef THEME
{
    m_font_holder.load(
        ui::Fonts::BASIC_SANS, HUD_FONT_SIZE,
        std::make_unique<ui::Font>(
            m_game.get_freetype(),
            "res/fonts/riffic_free.ttf",
            HUD_FONT_SIZE
        )
    );

    m_hud_font = &m_font_holder.get(ui::Fonts::BASIC_SANS, HUD_FONT_SIZE);

    init_explosion_pool  ();
    init_projectile_pool ();

    m_font_holder.load(
        ui::Fonts::BASIC_SANS, SCOREBOARD_TITLE_FONT_SIZE,
        std::make_unique<ui::Font>(
            m_game.get_freetype(),
            "res/fonts/riffic_free.ttf",
            SCOREBOARD_TITLE_FONT_SIZE
        )
    );

    m_font_holder.load(
        ui::Fonts::BASIC_SANS, SCOREBOARD_SCORE_FONT_SIZE,
        std::make_unique<ui::Font>(
            m_game.get_freetype(),
            "res/fonts/riffic_free.ttf",
            SCOREBOARD_SCORE_FONT_SIZE
        )
    );

    m_scoreboard = std::make_unique<ui::Scoreboard>(
        m_font_holder.get(ui::Fonts::BASIC_SANS, SCOREBOARD_TITLE_FONT_SIZE),
        m_font_holder.get(ui::Fonts::BASIC_SANS, SCOREBOARD_SCORE_FONT_SIZE)
    );

    m_kill_notifications = std::make_unique<ui::KillNotification>(
        m_font_holder.get(ui::Fonts::BASIC_SANS, HUD_FONT_SIZE),
        math::Vector2f({ 0.0f, static_cast<float>(Game::TARGET_DIMENSIONS[Y]) - 20.0f }),
        math::Vector4f({ 0, 0, 0, 1 })
    );

    m_fire_sounds[0] = &m_sound_holder.get(audio::Sounds::CANNON_FIRE);
    m_fire_sounds[1] = &m_sound_holder.get(audio::Sounds::SYRINGE_FIRE);
    m_fire_sounds[2] = &m_sound_holder.get(audio::Sounds::NEEDLER_FIRE);
}

GameplayState::~GameplayState()
{
    m_scoreboard = nullptr;
    m_font_holder.destroy(ui::Fonts::BASIC_SANS, SCOREBOARD_TITLE_FONT_SIZE);
    m_font_holder.destroy(ui::Fonts::BASIC_SANS, SCOREBOARD_SCORE_FONT_SIZE);
    m_hud_font = nullptr;
    m_font_holder.destroy(ui::Fonts::BASIC_SANS, HUD_FONT_SIZE);
    m_game.unload_level(m_level_id);
}

void GameplayState::on_enter()
{
    m_window.set_cursor_visible(false);
    graphics::RenderTools::set_clear_color(0.0f, 0.0f, 0.0f, 1.0f);
    update(std::chrono::milliseconds(0));
    //m_music.play();
}

void GameplayState::on_exit()
{
    m_music.stop();
}

void GameplayState::add_explosion(math::Vector2f const& pos, int const type)
{
    size_t i = 0;
    size_t size = m_explosions.size();
    while (i < size && m_explosions[i]->is_active())
        ++i;

    auto& explosion = m_explosions[i];

    explosion->activate();
    explosion->set_position(math::Vector2i(pos));
    explosion->set_source_position(
        math::Vector2i({
            graphics::EXPLOSION_OFFSET_X + type * graphics::EXPLOSION_DIMENSIONS,
            graphics::EXPLOSION_OFFSET_Y
        })
    );
    if (type == 3 /* Plane explosion ID */) 
        explosion->set_dimensions(EXPLOSION_TEXTURE_DIMENSIONS * 2);
    else 
        explosion->set_dimensions(EXPLOSION_TEXTURE_DIMENSIONS);
}

void GameplayState::add_projectile(math::Vector2f const& pos, math::Vector2f const& dir,
                                   std::chrono::milliseconds const proj_lifetime,
                                   float const spd, int const bullet_type, int const dmg)
{
    size_t i = bullet_type < 1 ? 512 : 0;
    size_t size = m_projectiles.size();
    while (i < size && m_projectiles[i]->is_active())
        ++i;

    auto& proj = m_projectiles[i];

    proj->activate();
    proj->reset_elapsed_time();
    proj->set_lifetime(proj_lifetime);
    proj->set_type(bullet_type);
    proj->set_damage(dmg);
    proj->set_source_position(
       math::Vector2i({
           graphics::PROJECTILE_OFFSET_X,
           graphics::PROJECTILE_OFFSET_Y + bullet_type * graphics::PROJECTILE_DIMENSIONS
        })
    );
    proj->set_position(pos);
    proj->set_velocity(dir * spd);

    auto& snd = m_fire_sounds[bullet_type];
    auto hndl = snd->play();
    static float const MIN_DAMP_DISTANCE = 100.0f;
    float const dist = (pos - m_player.get_rigid_body().get_position()).magnitude();
    if (dist > MIN_DAMP_DISTANCE)
    {
        static float const MAX_HEARING_DISTANCE = 2500.0f;
        snd->set_gain(hndl, 1.0f - fminf(dist, MAX_HEARING_DISTANCE) / MAX_HEARING_DISTANCE);
    }
}

void GameplayState::update(std::chrono::milliseconds const dt)
{
    if (m_keyboard.was_key_pressed(GLFW_KEY_F4))
    {
        m_game.pop_state();
        return;
    }

    if (m_keyboard.was_key_pressed(GLFW_KEY_J))
        m_kill_notifications->add_notification("Hehe humiliated Hoho.");
    m_kill_notifications->update(dt);

    if (m_keyboard.is_key_held(GLFW_KEY_TAB))
        m_show_scoreboard = true;
    else
        m_show_scoreboard = false;

    update_ai(dt, m_terrain);
    update_player(dt);

    for (unsigned i = 0; i < m_players.size(); ++i)
        for (int j = 0; j < 2; ++j)
        {
            physics::Collision coll = m_player.get_rigid_body().get_collider()[i].check_collision
                (m_players[i]->get_rigid_body().get_collider()[j]);
            if (coll.get_mtv()[X] != 0 || coll.get_mtv()[Y] != 0)
            {
                math::Vector2f normal = coll.get_mtv().normalized();
                math::Vector2f point = coll.get_collider() == 1 ? m_player.get_rigid_body().
                    find_point_of_impact(normal) : m_players[i]->get_rigid_body().
                    find_point_of_impact(normal);
                m_player.get_rigid_body().resolve_collision(normal, point, m_players[i]->get_rigid_body());
                m_player.get_rigid_body().correct_positions(coll.get_mtv(), dt, m_players[i]->get_rigid_body(), m_terrain);
            }
        }

    for (unsigned i = 0; i < m_players.size() == 0 ? m_players.size() : m_players.size()-1; ++i)
        for (unsigned k = i+1; k < m_players.size(); ++k)
            for (int j = 0; j < 2; ++j)
            {
                physics::Collision coll = m_players[i]->get_rigid_body().get_collider()[i].check_collision
                    (m_players[k]->get_rigid_body().get_collider()[j]);
                if (coll.get_mtv()[X] != 0 || coll.get_mtv()[Y] != 0)
                {
                    math::Vector2f normal = coll.get_mtv().normalized();
                    math::Vector2f point = coll.get_collider() == 1 ? m_players[i]->get_rigid_body().
                        find_point_of_impact(normal) : m_players[k]->get_rigid_body().
                        find_point_of_impact(normal);
                    m_players[i]->get_rigid_body().resolve_collision(normal, point, m_players[k]->get_rigid_body());
                    m_players[i]->get_rigid_body().correct_positions(coll.get_mtv(), dt, m_players[k]->get_rigid_body(), m_terrain);
                }
            }
    for (auto& e : m_explosions)
        if (e->is_active())
            e->update(dt);

    for (auto& p : m_projectiles)
        if (p->is_active())
        {
            p->update(dt, m_terrain);
            for (unsigned i = 0; i < m_players.size(); ++i)
            {
                for (int j = 0; j < 2; ++j)
                {
                    math::Vector2f coll = p->check_collision(m_players[i]->get_rigid_body().get_collider()[j]);
                    if (coll[X] != 0 || coll[Y] != 0)
                    {
                        m_players[i]->get_plane().take_damage(p->get_damage());
                        bool show_explosion = 1;
                        if (m_players[i]->get_plane().get_health().value <= 0)
                        {
                            add_explosion(m_players[i]->get_plane().get_position(), 3);
                            show_explosion = 0;
                        }
                        p->resolve_collision(coll, show_explosion);
                        break;
                    }
                }
            }
        }

    update_terrain();
    update_transform();
    update_crosshair(dt);
}

void GameplayState::render()
{
    m_batch_renderer.begin();
    m_batch_renderer.submit(m_background_sprite);
    m_batch_renderer.submit(m_terrain_sprite);
    m_batch_renderer.submit(m_player.get_sprite());
    m_batch_renderer.submit(m_ai.get_sprite());
    for (auto& s : m_explosions)
        if (s->is_active())
            m_batch_renderer.submit(*s);

    for (auto& p : m_projectiles)
        if (p->is_active())
            m_batch_renderer.submit(p->get_sprite());
    m_batch_renderer.submit(m_player.get_weapon_sprite());
    m_batch_renderer.submit(m_ai.get_weapon_sprite());
    m_batch_renderer.submit(m_crosshair_sprite);
    m_batch_renderer.end();
    m_batch_renderer.flush();

    m_font_renderer.render_shadowed_text(*m_hud_font, m_ammo_text, m_ammo_text_position,
                                         math::Vector2f({ 1.0f, -1.0f }));
    m_font_renderer.render_shadowed_text(*m_hud_font, m_health_text, m_health_text_position,
                                         math::Vector2f({ 1.0f, -1.0f }));
    m_font_renderer.render_shadowed_text(*m_hud_font, m_current_weapon_text,
                                         m_current_weapon_text_position,
                                         math::Vector2f({ 1.0f, -1.0f }));

    m_kill_notifications->render_text(m_font_renderer);

    if (m_show_scoreboard)
        m_scoreboard->render(m_font_renderer);
}

void GameplayState::update_ai(std::chrono::milliseconds const dt, entities::LevelTerrain& terr)
{
    math::Vector2f cursor_pos = m_window.mouse.get_cursor_position();
    m_ai.handle_input(cursor_pos, m_keyboard, m_mouse, m_mouse_world_position, dt);
    m_ai.update(dt, terr);
    m_ai.update_behavior(m_player.get_rigid_body().get_position(), dt);
}

void GameplayState::update_player(std::chrono::milliseconds const dt)
{
    m_player.handle_input(m_keyboard, m_mouse, m_mouse_world_position, dt);
    m_player.update(dt, m_terrain);

    auto& health = m_player.get_health();
    int curr_health = static_cast<int>(
        static_cast<float>(health.value) / static_cast<float>(health.max_value) * 100.0f
    );
    if (curr_health != m_previous_health)
    {
        m_health_text = std::to_string(curr_health) + "%";
        m_previous_health = curr_health;
    }

    auto& curr_weap = m_player.get_weapon();

    int const curr_ammo_cnt = curr_weap.get_rounds();
    if (curr_ammo_cnt != m_previous_ammo_count)
    {
        m_ammo_text = std::to_string(curr_ammo_cnt) + "/" 
                    + std::to_string(curr_weap.get_magazine_size());
        float const ammo_text_w = m_hud_font->get_text_width(m_ammo_text);
        m_ammo_text_position = math::Vector2f({
            static_cast<float>(Game::TARGET_DIMENSIONS[X]) - 20.0f - ammo_text_w,
            20.0f
        });
        m_previous_ammo_count = curr_ammo_cnt;
    }

    int const curr_weap_idx = curr_weap.get_index();
    if (curr_weap_idx != m_current_weapon_index)
    {
        m_current_weapon_text = curr_weap.get_name();
        if (curr_weap.is_automatic())
            m_current_weapon_text += " (auto)";
        m_current_weapon_index = curr_weap_idx;
        float const weap_text_w = m_hud_font->get_text_width(m_current_weapon_text);
        m_current_weapon_text_position = math::Vector2f({
            static_cast<float>(Game::TARGET_DIMENSIONS[X] / 2) - weap_text_w / 2.0f,
            20.0f
        });
    }
}

void GameplayState::update_terrain()
{
    m_terrain.update();
}

void GameplayState::update_transform()
{
    math::Vector2f const tgt_dimf(Game::TARGET_DIMENSIONS);
    math::Vector2f const tgt_dimf_half(tgt_dimf / 2.0f);
    math::Vector2f const terrain_dimf(m_terrain_dimensions);
    math::Vector2f const plr_pos = m_player.get_rigid_body().get_position();
    m_camera_position = math::Vector2f({
        math::clamp(plr_pos[X] - tgt_dimf_half[X], 0.0f, terrain_dimf[X] - tgt_dimf[X]),
        math::clamp(plr_pos[Y] - tgt_dimf_half[Y], 0.0f, terrain_dimf[Y] - tgt_dimf[Y])
    });
    graphics::RenderTools::set_transform(m_batch_renderer.get_shader(), m_camera_position);

    m_mouse_world_position = m_mouse.get_cursor_world_position(m_camera_position);
}

void GameplayState::update_crosshair(std::chrono::milliseconds const dt)
{
    m_crosshair_sprite.set_position(math::Vector2i(m_mouse_world_position));

    if (m_player.get_weapon().get_rounds() == 0 || m_player.is_reloading())
        m_crosshair_sprite.set_source_position(math::Vector2i::zero());
    else
        m_crosshair_sprite.set_source_position(
            math::Vector2i({ 0, m_crosshair_sprite.get_source_dimensions()[X] })
        );

    if (m_player.is_reloading())
    {
        float const t        = static_cast<float>(dt.count())
                             * 0.001f;
        float const reload_t = static_cast<float>(m_player.get_weapon().get_reload_time().count())
                             * 0.001f;
        float rot = m_crosshair_sprite.get_rotation() + (t / reload_t) * math::DOUBLE_PI;
        rot += rot >  math::DOUBLE_PI ? -math::DOUBLE_PI
             : rot < -math::DOUBLE_PI ?  math::DOUBLE_PI
             :                           0.0f;
        m_crosshair_sprite.set_rotation(rot);
    }
    else
    {
        m_crosshair_sprite.set_rotation(0.0f);
    }
}

void GameplayState::init_explosion_pool()
{
    for (int i = 0; i < 1024; ++i)
        m_explosions.push_back(std::make_unique<graphics::Explosion>(
            m_gameplay_elements_texture,
            EXPLOSION_TEXTURE_POSITION, EXPLOSION_TEXTURE_DIMENSIONS,
            math::Vector2i::zero(), EXPLOSION_TEXTURE_DIMENSIONS,
            math::Vector2i::up() * graphics::EXPLOSION_DIMENSIONS,
            std::chrono::milliseconds(50), 16,
            math::Vector2f({ 0.5f, 0.5f }))
        );
}

void GameplayState::init_projectile_pool()
{
    for (int i = 0; i < 512; ++i)
        m_projectiles.push_back(std::make_unique<entities::BasicProjectile>(
            m_texture_holder.get(graphics::Textures::GAMEPLAY_ELEMENTS), this)
        );

    for (int i = 0; i < 512; ++i)
        m_projectiles.push_back(std::make_unique<entities::TrajectoryProjectile>(
            m_texture_holder.get(graphics::Textures::GAMEPLAY_ELEMENTS), this)
        );
}

} // namespace logic
