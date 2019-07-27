#ifndef LOGIC_CLIENT_LOBBY_STATE_HPP
#define LOGIC_CLIENT_LOBBY_STATE_HPP

#include <memory>

#include "state.hpp"
#include "../graphics/sprite.hpp"
#include "../graphics/texture.hpp"
#include "../network/socket/client_socket_win.hpp"
#include "../network/connection.hpp"
#include "../ui/button.hpp"
#include "../ui/input_field.hpp"
#include "../utilities/async_queue.hpp"

namespace network {

class Player;
class UdpPacket;

} // network

namespace logic {

class ClientLobbyState final : public State
{
private:
    static int const M_NUM_BUTTONS = 1;

    enum Button
    {
        BUTTON_READY
    };

    int  m_selected_level;
    bool m_ready;

    ui::Font*                       m_font;
    ui::Font*                       m_small_font;
    graphics::Texture&              m_button_texture;
    std::unique_ptr<ui::Button>     m_buttons[M_NUM_BUTTONS];
    std::unique_ptr<ui::InputField> m_name_input;

    std::unique_ptr<graphics::Sprite> m_thumbnail_sprite;
    math::Vector2f                    m_thumbnail_text_position;
    math::Vector2f                    m_connections_text_position;

    std::thread           m_listener_thread;
    unsigned char         m_public_id;

    std::unique_ptr<network::ClientSocket> m_socket;

    std::vector<std::shared_ptr<network::Connection>> m_connections;
    utilities::AsyncQueue<std::pair<network::UdpPacket, sockaddr_in>> m_udp_queue;

    audio::Music& m_music;

public:
    explicit  ClientLobbyState(Game& g, std::string address, unsigned short const port);
             ~ClientLobbyState();

    ClientLobbyState            (ClientLobbyState const&) = delete;
    ClientLobbyState& operator= (ClientLobbyState const&) = delete;

    void on_enter () override;
    void on_exit  () override;

    void update(std::chrono::milliseconds const dt) override;
    void render() override;

private:
    void update_thumbnail();

    void open_socket();
    void close_socket();

    void add_player       (unsigned char const team,
                           unsigned char const pos,
                           std::string const name);
    void drop_player      (std::shared_ptr<network::Connection> player);
    void process_packet   (network::UdpPacket packet);
    void process_udp_queue();
};

} // namespace logic

#endif // LOGIC_CLIENT_LOBBY_STATE_HPP
