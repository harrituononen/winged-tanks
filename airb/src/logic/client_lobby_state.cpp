#include "client_lobby_state.hpp"

#include "game.hpp"
#include "level_data.hpp"
#include "../graphics/render_tools.hpp"
#include "../graphics/texture.hpp"
#include "../network/utilities/config.hpp"
#include "../network/utilities/udp_packet.hpp"

namespace logic {

ClientLobbyState::ClientLobbyState(Game& g, std::string address, unsigned short const port)
    : State (g)

    , m_selected_level (0)
    , m_ready(false)

    , m_font           (nullptr)
    , m_small_font     (nullptr)
    , m_button_texture (m_texture_holder.get(graphics::Textures::UI))
    , m_buttons        ()
    , m_name_input     (nullptr)

    , m_thumbnail_sprite        (nullptr)
    , m_thumbnail_text_position ({ 0.0f,
                                   static_cast<float>(
                                       Game::TARGET_DIMENSIONS[Y] / 2
                                     - THUMBNAIL_DIMENSIONS[Y] / 2
                                     - BUTTON_FONT_SIZE - 10
                                   ) })
    , m_connections_text_position ({
        60.0f, Game::TARGET_DIMENSIONS[Y] - static_cast<float>(TEAM_INFO_FONT_SIZE) - 60.0f
    })
    , m_socket(std::make_unique<network::ClientSocket>(address, port))
    , m_public_id(0)

    , m_music(m_music_holder.get(audio::MusicThemes::LOBBY))
{
    open_socket();

    m_font_holder.load(
        ui::Fonts::BASIC_SANS,
        BUTTON_FONT_SIZE,
        std::make_unique<ui::Font>(
            m_game.get_freetype(), "res/fonts/riffic_free.ttf", BUTTON_FONT_SIZE
        )
    );
    m_font_holder.load(
        ui::Fonts::BASIC_SANS,
        TEAM_INFO_FONT_SIZE,
        std::make_unique<ui::Font>(
            m_game.get_freetype(), "res/fonts/riffic_free.ttf", TEAM_INFO_FONT_SIZE
        )
    );

    m_font       = &m_font_holder.get(ui::Fonts::BASIC_SANS, BUTTON_FONT_SIZE);
    m_small_font = &m_font_holder.get(ui::Fonts::BASIC_SANS, TEAM_INFO_FONT_SIZE);
    int const btn_h         = 100 + BUTTON_FONT_SIZE;
    int const start_btn_w   = 400;
    int const rl_btn_w      = 100;
    int const rl_btn_offset = THUMBNAIL_DIMENSIONS[X] / 2;

    m_buttons[BUTTON_READY] = std::make_unique<ui::Button>(
        *m_font,
        "Ready",
        math::Vector4f::one(),
        m_button_texture,
        math::Vector2i::zero(),
        m_button_texture.get_dimensions(),
        math::Vector2i({ Game::TARGET_DIMENSIONS[X] - start_btn_w - 60, 60 }),
        math::Vector2i({ start_btn_w, btn_h })
    );

    m_name_input = std::make_unique<ui::InputField>(
        *m_font, DEFAULT_PLAYER_NAMES[rand() % NUM_DEFAULT_PLAYER_NAMES], 12,
        math::Vector2f::one() * 60.0f,
        std::chrono::milliseconds(750),
        "Name:"
    );
    m_texture_holder.load(
        graphics::Textures::LEVEL_THUMBNAILS,
        std::make_unique<graphics::Texture>("res/textures/levels/thumbnails.tga")
    );

    m_thumbnail_sprite = std::make_unique<graphics::Sprite>(
        m_texture_holder.get(graphics::Textures::LEVEL_THUMBNAILS),
        math::Vector2i::zero(),
        THUMBNAIL_SOURCE_DIMENSIONS,
        Game::TARGET_DIMENSIONS / 2,
        THUMBNAIL_DIMENSIONS,
        math::Vector2f({ 0.5f, 0.5f })
    );

    update_thumbnail();
}

ClientLobbyState::~ClientLobbyState()
{
    if (m_socket != nullptr)
    {
        m_socket->send(network::UdpPacket(network::UDP_H_CONNECT |
                                          network::UDP_H_ERROR |
                                          network::UDP_H_DATABLOCK,
                                          m_socket->get_client_id()));
        close_socket();
    }

    for (int i = 0; i < M_NUM_BUTTONS; ++i)
        m_buttons[i] = nullptr;
    m_name_input = nullptr;
    m_font = nullptr;
    m_small_font = nullptr;
    m_font_holder.destroy(ui::Fonts::BASIC_SANS, BUTTON_FONT_SIZE);
    m_font_holder.destroy(ui::Fonts::BASIC_SANS, TEAM_INFO_FONT_SIZE);

    m_thumbnail_sprite = nullptr;
    m_texture_holder.destroy(graphics::Textures::LEVEL_THUMBNAILS);
}

void ClientLobbyState::on_enter()
{
    m_socket->init_connect();
    graphics::RenderTools::set_clear_color(0.0f, 0.25f, 0.5f, 1.0f);
    m_music.play();
}

void ClientLobbyState::on_exit()
{
    m_music.stop();
}

void ClientLobbyState::update(std::chrono::milliseconds const dt)
{
    m_name_input->update(dt, m_keyboard);

    for (int i = 0; i < M_NUM_BUTTONS; ++i)
    {
        auto& btn = m_buttons[i];

        bool const hovered = btn->is_point_inside(
            math::Vector2i(m_mouse.get_cursor_world_position(math::Vector2f::zero()))
        );
        if (hovered)
        {
            btn->get_sprite().set_color(math::Vector4f({ 0.0f, 0.0f, 0.0f, 1.0f }));

            if (m_mouse.was_button_pressed(GLFW_MOUSE_BUTTON_1))
            {
                if (i == BUTTON_READY)
                {
                    m_ready = !m_ready;
                    m_socket->send(network::UdpPacket(network::UDP_H_OK  |
                                                     network::UDP_H_POS |
                                                     network::UDP_H_DATABLOCK,
                                                     m_public_id,
                                                     const_cast<char*>(m_name_input->get_value().c_str())
                                                     ));
                    m_name_input->set_enabled(!m_ready);
                    return;
                }
            }
        }
        else
        {
            btn->get_sprite().set_color(math::Vector4f::one());
        }
    }

    process_udp_queue();

    if (m_keyboard.was_key_pressed(GLFW_KEY_F4))
    {
        close_socket(); // Apparently something here causes a crash...
        m_game.pop_state();
        return;
    }
}

void ClientLobbyState::render()
{
    m_batch_renderer.begin();
    m_batch_renderer.submit(*m_thumbnail_sprite);
    for (int i = 0; i < M_NUM_BUTTONS; ++i)
        m_batch_renderer.submit(m_buttons[i]->get_sprite());
    m_batch_renderer.end();
    m_batch_renderer.flush();

    for (int i = 0; i < M_NUM_BUTTONS; ++i)
        m_buttons[i]->render_text(m_font_renderer);
    m_font_renderer.render_shadowed_text(
        *m_font,
        LEVEL_NAMES[m_selected_level],
        m_thumbnail_text_position,
        math::Vector2f({ 1.0f, -1.0f })
    );

    if (m_connections.size() > 0)
    {
        m_font_renderer.render_shadowed_text(
            *m_small_font,
            "Blue team",
            m_connections_text_position,
            math::Vector2f({ 1.0f, -1.0f }),
            math::Vector4f({ 0.0f, 0.5f, 1.0f, 1.0f })
        );
        m_font_renderer.render_shadowed_text(
            *m_small_font,
            "Red team",
            m_connections_text_position + math::Vector2f({ RED_TEAM_OFFSET, 0.0f }),
            math::Vector2f({ 1.0f, -1.0f }),
            math::Vector4f({ 1.0f, 0.0f, 0.0f, 1.0f })
        );
        m_font_renderer.render_shadowed_text(
            *m_small_font,
            m_connections[0]->get_name(),
            m_connections_text_position - math::Vector2f({ 0.0f, 40.0f }),
            math::Vector2f({ 1.0f, -1.0f }),
            math::Vector4f::one()
        );
        auto c_blue = 1;
        auto c_red  = 1;
        for (auto player : m_connections)
        {
            auto pos   = m_connections_text_position;
            auto color = math::Vector4f::one();
            auto name  = player->get_name(); 
            if (player->get_public_id() & network::UDP_POS_TEAM_1)
            {
                pos += math::Vector2f({ 0.0f, -40.0f * c_blue++ });
            }
            else
            {
                pos += math::Vector2f({ RED_TEAM_OFFSET, -40.0f * c_red++ });
            }
            if (player->get_status() == network::Connection::Status::LOBBY_READY)
            {
                color = math::Vector4f({ 0.0f, 1.0f, 0.0f, 1.0f });
            }
            if (player->get_public_id() == m_public_id)
                name = m_name_input->get_value();

            m_font_renderer.render_shadowed_text(
                *m_small_font,
                name,
                pos,
                math::Vector2f({ 1.0f, -1.0f }),
                color
            );
        }
    }

    m_name_input->render_text(m_font_renderer);
}

void ClientLobbyState::update_thumbnail()
{
    m_thumbnail_sprite->set_source_position(math::Vector2i({
        0, static_cast<int>(m_selected_level) * THUMBNAIL_SOURCE_DIMENSIONS[Y]
    }));

    float const txt_w = m_font->get_text_width(LEVEL_NAMES[m_selected_level]);
    m_thumbnail_text_position[X] = static_cast<float>(Game::TARGET_DIMENSIONS[X] / 2)
                                 - txt_w / 2.0f;
}

void ClientLobbyState::open_socket()
{
    m_listener_thread = std::thread([&]
    {
        m_socket->start_listener_routine();
    });
}

void ClientLobbyState::close_socket()
{
    m_socket->end_listener_routine();
    m_socket->close();
    m_listener_thread.join();
}

void ClientLobbyState::add_player(unsigned char const team,
                                  unsigned char const pos,
                                  std::string const name)
{
    auto new_player(std::make_shared<network::Connection>(team, pos));
    new_player->set_status(network::Connection::Status::CONNECTED);
    // Auto set host ready
    if (team == network::UDP_POS_TEAM_1 && pos == network::UDP_POS_PLAYER_1)
        new_player->set_status(network::Connection::Status::LOBBY_READY);
    new_player->set_name(name);
    m_connections.push_back(new_player);
}

void ClientLobbyState::drop_player(std::shared_ptr<network::Connection> player)
{
    m_connections.erase(remove(m_connections.begin(),
                               m_connections.end(),
                               player),
                               m_connections.end());
}

void ClientLobbyState::process_packet(network::UdpPacket packet)
{
    if (packet.header_contains(network::UDP_H_CONNECT) &&
        packet.header_contains(network::UDP_H_OK) &&
        packet.header_contains(network::UDP_H_DATABLOCK) &&
        packet.header_contains(network::UDP_H_POS))
    {
        // Host accept connection
        m_public_id = packet.get_pos_byte();
        // -- DEBUG Below is for debugging only, safe to remove
        auto team = m_public_id & network::UDP_POS_TEAM_1   ? 1 : 2;
        auto pnum = m_public_id & network::UDP_POS_PLAYER_1 ? 1 :
                    m_public_id & network::UDP_POS_PLAYER_2 ? 2 :
                    m_public_id & network::UDP_POS_PLAYER_3 ? 3 :
                    m_public_id & network::UDP_POS_PLAYER_4 ? 4 : 0;
        printf("-- DEBUG -- I'm on team %d and my player number is %d", team, pnum);
        // -- DEBUG end
        return;
    }
    if (packet.header_contains(network::UDP_H_CONNECT) &&
        packet.header_contains(network::UDP_H_POS) &&
        packet.header_contains(network::UDP_H_DATABLOCK))
    {
        // Receive lobby players
        auto pos = packet.get_pos_byte();
        auto team = pos & network::UDP_POS_TEAM_1 ? network::UDP_POS_TEAM_1 : network::UDP_POS_TEAM_2;
        pos ^= team;
        add_player(team, pos, packet.get_data_block());
        return;
    }
    if (packet.header_contains(network::UDP_H_CONNECT) &&
        packet.header_contains(network::UDP_H_ERROR) &&
        packet.header_contains(network::UDP_H_POS))
    {
        for (auto player : m_connections)
        {
            if (!packet.position_is(player->get_public_id()))
                continue;

            drop_player(player);
            break;
        }
        return;
    }
    if (packet.header_contains(network::UDP_H_OK) &&
        packet.header_contains(network::UDP_H_POS) &&
        packet.header_contains(network::UDP_H_DATABLOCK))
    {
        for (auto player : m_connections)
        {
            if (!packet.position_is(player->get_public_id()))
                continue;

            player->set_name(packet.get_data_block());

            // Host doesn't change status
            if (player->get_public_id() == (network::UDP_POS_TEAM_1 | network::UDP_POS_PLAYER_1))
                break;

            if (player->get_status() == network::Connection::Status::CONNECTED)
                player->set_status(network::Connection::Status::LOBBY_READY);
            else
                player->set_status(network::Connection::Status::CONNECTED);
            break;
        }
        return;
    }
    if (packet.header_contains(network::UDP_H_KEEPALIVE) &&
        packet.header_contains(network::UDP_H_DATABLOCK))
    {
        // Packet data is by default 0, so server needs to send level as
        // one higher than it actually is, hence "- 1"
        std::string level(packet.get_data_block());
        m_selected_level = std::stoi(level) - 1;
        update_thumbnail();
        return;
    }
    if (packet.header_contains(network::UDP_H_KEEPALIVE) &&
        packet.header_contains(network::UDP_H_OK))
    {
        // Remove self
        for (auto player : m_connections)
        {
            if (player->get_public_id() != m_public_id)
                continue;

            drop_player(player);
            break;
        }
        // Start game
        m_socket->end_listener_routine();
        m_socket->close();
        m_listener_thread.join();
        m_game.join_game(LEVEL_IDS[m_selected_level],
                         LEVEL_PATHS[m_selected_level],
                         std::move(m_socket),
                         m_public_id,
                         m_connections);
        return;
    }
}

void ClientLobbyState::process_udp_queue()
{
    while (m_socket->has_packets())
    {
        auto packet = m_socket->get_packet();
        process_packet(packet);

        if (packet.header_contains(network::UDP_H_KEEPALIVE) &&
            packet.header_contains(network::UDP_H_OK))
        {
            return;
        }
    }
}

} // namespace logic
