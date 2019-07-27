#ifndef LOGIC_SERVER_GAMEPLAY_STATE_HPP
#define LOGIC_SERVER_GAMEPLAY_STATE_HPP

#include <chrono>
#include <memory>

#include "gameplay_state.hpp"
#include "../network/player.hpp"
#include "../network/socket/server_socket_win.hpp"
#include "../utilities/async_queue.hpp"

namespace logic {

namespace
{
    int const HUD_FONT_SIZE = 48;

    int const SCOREBOARD_TITLE_FONT_SIZE = 96;
    int const SCOREBOARD_SCORE_FONT_SIZE = 40;

    math::Vector2i const PLANE_TEXTURE_POSITION({ graphics::PLANE_OFFSET_X,
                                                  graphics::PLANE_OFFSET_Y });
    math::Vector2i const PLANE_TEXTURE_DIMENSIONS({ graphics::PLANE_DIMENSIONS,
                                                    graphics::PLANE_DIMENSIONS });
    math::Vector2i const EXPLOSION_TEXTURE_POSITION({ graphics::EXPLOSION_OFFSET_X,
                                                      graphics::EXPLOSION_OFFSET_Y });
    math::Vector2i const EXPLOSION_TEXTURE_DIMENSIONS({ graphics::EXPLOSION_DIMENSIONS,
                                                        graphics::EXPLOSION_DIMENSIONS });
}

class ServerGameplayState final : public GameplayState
{
private:
    std::thread           m_conn_check_thread;
    std::thread           m_cursor_sync_thread;
    std::thread           m_listener_thread;
    std::thread           m_pos_sync_thread;
    bool                  m_pos_sync_running;
    bool                  m_cursor_sync_running;
    unsigned char         m_public_id;
    bool                  m_waiting_for_players;

    std::unique_ptr<network::ServerSocket> m_socket;

public:
    ServerGameplayState(Game& g,
                        graphics::Textures const lvl_id,
                        std::unique_ptr<network::ServerSocket>& socket,
                        std::vector<std::shared_ptr<network::Connection>> players);
    ~ServerGameplayState();

    ServerGameplayState            (ServerGameplayState const&) = delete;
    ServerGameplayState& operator= (ServerGameplayState const&) = delete;

    void on_enter () override;
    void on_exit  () override;

    void update(std::chrono::milliseconds const dt) override;
    void render() override;

private:
    void update_player    (std::chrono::milliseconds const dt) override;

    void update_pressed_keys () const;
    void update_released_keys() const;

    void open_socket();
    void close_socket();

    void add_player       (network::Connection conn);
    void drop_player      (std::shared_ptr<network::Player> player);
    void process_packet   (network::UdpPacket packet, sockaddr_in client_address);
    void process_udp_queue();

    void start_pos_sync_routine   ();
    void start_cursor_sync_routine();
    void end_pos_sync_routine     ();
    void end_cursor_sync_routine  ();
};

} // namespace logic

#endif // LOGIC_SERVER_GAMEPLAY_STATE_HPP
