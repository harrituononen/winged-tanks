#ifndef LOGIC_CLIENT_GAMEPLAY_STATE_HPP
#define LOGIC_CLIENT_GAMEPLAY_STATE_HPP

#include <chrono>
#include <memory>

#include "gameplay_state.hpp"
#include "../network/player.hpp"
#include "../network/socket/client_socket_win.hpp"
#include "../utilities/async_queue.hpp"

namespace logic {

class ClientGameplayState final : public GameplayState
{
private:
    std::thread           m_keepalive_thread;
    std::thread           m_listener_thread;
    std::thread           m_cursor_sync_thread;
    bool                  m_cursor_sync_running;
    bool                  m_host_quit;
    unsigned char         m_public_id;
    bool                  m_waiting_for_players;

    std::unique_ptr<network::ClientSocket> m_socket;

    utilities::AsyncQueue<network::UdpPacket> m_udp_queue;

public:
     ClientGameplayState(Game& g,
                         graphics::Textures const lvl_id,
                         std::unique_ptr<network::ClientSocket> socket,
                         unsigned char const public_id,
                         std::vector<std::shared_ptr<network::Connection>> players);
    ~ClientGameplayState();

     ClientGameplayState            (ClientGameplayState const&) = delete;
     ClientGameplayState& operator= (ClientGameplayState const&) = delete;

    void on_enter () override;
    void on_exit  () override;

    void update(std::chrono::milliseconds const dt) override;
    void render() override;

private:
    void update_player(std::chrono::milliseconds const dt) override;

    void update_pressed_keys ();
    void update_released_keys();

    void open_socket ();
    void close_socket();

    void add_player       (network::Connection conn);
    void drop_player      (std::shared_ptr<network::Player> player);
    void process_packet   (network::UdpPacket packet);
    void process_udp_queue();

    void start_cursor_sync_routine();
    void end_cursor_sync_routine  ();
};

} // namespace logic

#endif // LOGIC_CLIENT_GAMEPLAY_STATE_HPP
