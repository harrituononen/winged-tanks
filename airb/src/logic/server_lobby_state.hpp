#ifndef LOGIC_SERVER_LOBBY_STATE_HPP
#define LOGIC_SERVER_LOBBY_STATE_HPP

#include <memory>

#include "state.hpp"
#include "../graphics/sprite.hpp"
#include "../graphics/texture.hpp"
#include "../network/connection.hpp"
#include "../network/utilities/udp_packet.hpp"
#include "../network/socket/server_socket_win.hpp"
#include "../ui/button.hpp"
#include "../ui/input_field.hpp"
#include "../utilities/async_queue.hpp"

namespace network {

class Player;

} // network

namespace logic {

class ServerLobbyState final : public State
{
private:
    static int const M_NUM_BUTTONS = 4;

    enum Button
    {
        BUTTON_LEFT,
        BUTTON_RIGHT,
        BUTTON_START,
        BUTTON_OK
    };

    int m_selected_level;

    ui::Font*                       m_font;
    ui::Font*                       m_small_font;
    graphics::Texture&              m_button_texture;
    std::unique_ptr<ui::Button>     m_buttons[M_NUM_BUTTONS];
    std::unique_ptr<ui::InputField> m_name_input;

    std::unique_ptr<graphics::Sprite> m_thumbnail_sprite;
    math::Vector2f                    m_thumbnail_text_position;
    math::Vector2f                    m_connections_text_position;

    std::thread           m_listener_thread;
    unsigned char         m_id;

    std::unique_ptr<network::ServerSocket> m_socket;

    std::vector<std::shared_ptr<network::Connection>> m_connections;
    utilities::AsyncQueue<std::pair<network::UdpPacket, sockaddr_in>> m_udp_queue;

    audio::Music& m_music;

public:
    explicit  ServerLobbyState(Game& g, unsigned short const port);
             ~ServerLobbyState();

    ServerLobbyState            (ServerLobbyState const&) = delete;
    ServerLobbyState& operator= (ServerLobbyState const&) = delete;

    void on_enter () override;
    void on_exit  () override;

    void update(std::chrono::milliseconds const dt) override;
    void render() override;

private:
    void update_thumbnail();

    void open_socket();
    void close_socket();

    void add_player       (sockaddr_in client_address);
    void assign_team      (std::shared_ptr<network::Connection> player) const;
    void drop_player      (std::shared_ptr<network::Connection> player);
    void process_packet   (network::UdpPacket packet, sockaddr_in client_address);
    void process_udp_queue();

    unsigned char get_open_position(unsigned char const team) const;
    int           get_player_count (unsigned char const team) const;
};

} // namespace logic

#endif // LOGIC_SERVER_LOBBY_STATE_HPP
