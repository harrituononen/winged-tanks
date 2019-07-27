#ifndef LOGIC_GAMEPLAY_STATE_HPP
#define LOGIC_GAMEPLAY_STATE_HPP

#include <chrono>
#include <memory>

#include "state.hpp"
#include "../audio/sound.hpp"
#include "../entities/ai_plane.hpp"
#include "../entities/controllable_plane.hpp"
#include "../entities/level_terrain.hpp"
#include "../entities/basic_projectile.hpp"
#include "../entities/trajectory_projectile.hpp"
#include "../graphics/explosion.hpp"
#include "../graphics/sprite.hpp"
#include "../graphics/texture.hpp"
#include "../ui/font.hpp"
#include "../ui/kill_notification.hpp"
#include "../ui/scoreboard.hpp"
#include "../math/matrix.hpp"
#include "../network/player.hpp"
#include "../network/utilities/udp_packet.hpp"

namespace graphics {

class Texture;

} // namespace graphics

namespace logic {

class GameplayState : public State
{
protected:
    graphics::Textures const m_level_id;

    graphics::Texture&          m_gameplay_elements_texture;
    graphics::Texture&          m_level_texture;
    graphics::Texture&          m_crosshair_texture;
    entities::AiPlane           m_ai;
    entities::ControllablePlane m_player;
    entities::LevelTerrain      m_terrain;

    math::Vector2i              m_terrain_dimensions;
    graphics::Sprite            m_terrain_sprite;
    graphics::Sprite            m_background_sprite;
    graphics::Sprite            m_crosshair_sprite;

    math::Vector2f m_camera_position;
    math::Vector2f m_mouse_world_position;

    std::vector<std::unique_ptr<entities::Projectile>> m_projectiles;
    std::vector<std::unique_ptr<graphics::Explosion>>  m_explosions;

    ui::Font* m_hud_font;

    std::unique_ptr<ui::KillNotification> m_kill_notifications;
    std::unique_ptr<ui::Scoreboard>       m_scoreboard;
    bool                                  m_show_scoreboard;

    int            m_previous_ammo_count;
    std::string    m_ammo_text;
    math::Vector2f m_ammo_text_position;

    int            m_previous_health;
    std::string    m_health_text;
    math::Vector2f m_health_text_position;

    int            m_current_weapon_index;
    std::string    m_current_weapon_text;
    math::Vector2f m_current_weapon_text_position;

    std::vector<std::shared_ptr<network::Player>> m_players;

    audio::Sound* m_fire_sounds[3];
    audio::Music& m_music;

public:
     GameplayState(Game& g, graphics::Textures const lvl_id);
    ~GameplayState();

    GameplayState            (GameplayState const&) = delete;
    GameplayState& operator= (GameplayState const&) = delete;

    void add_explosion  (math::Vector2f const& pos, int const type);
    void add_projectile (math::Vector2f const& pos, math::Vector2f const& dir,
                         std::chrono::milliseconds const proj_lifetime,
                         float const spd, int const bullet_type, int const dmg);

    void on_enter () override;
    void on_exit  () override;

    void update(std::chrono::milliseconds const dt) override;
    void render() override;

protected:
    virtual void update_ai(std::chrono::milliseconds const dt, entities::LevelTerrain& terr);
    virtual void update_player(std::chrono::milliseconds const dt);

private:
    void update_terrain   ();
    void update_transform ();
    void update_crosshair (std::chrono::milliseconds const dt);

    void init_explosion_pool  ();
    void init_projectile_pool ();
};

} // namespace logic

#endif // LOGIC_GAMEPLAY_STATE_HPP
