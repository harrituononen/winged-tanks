#include "server_lobby_state.hpp"

#include <ctime>

#include "game.hpp"
#include "level_data.hpp"
#include "../graphics/render_tools.hpp"
#include "../graphics/texture.hpp"
#include "../network/utilities/config.hpp"

namespace logic {

ServerLobbyState::ServerLobbyState(Game& g, unsigned short const port)
    : State (g)

    , m_selected_level (0)

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
    , m_id(network::UDP_POS_TEAM_1 | network::UDP_POS_PLAYER_1)
    , m_socket(std::make_unique<network::ServerSocket>(port))

    , m_music (m_music_holder.get(audio::MusicThemes::LOBBY))
{
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

    m_buttons[BUTTON_START] = std::make_unique<ui::Button>(
        *m_font,
        "Start",
        math::Vector4f::one(),
        m_button_texture,
        math::Vector2i::zero(),
        m_button_texture.get_dimensions(),
        math::Vector2i({ Game::TARGET_DIMENSIONS[X] - start_btn_w - 60, 60 }),
        math::Vector2i({ start_btn_w, btn_h })
    );
    m_buttons[BUTTON_START]->get_sprite().set_color(math::Vector4f({ 1.0f, 0.5f, 0.0f, 1.0f }));

    m_buttons[BUTTON_LEFT] = std::make_unique<ui::Button>(
        *m_font,
        "<",
        math::Vector4f::one(),
        m_button_texture,
        math::Vector2i::zero(),
        m_button_texture.get_dimensions(),
        math::Vector2i({ Game::TARGET_DIMENSIONS[X] / 2 - rl_btn_offset - rl_btn_w,
                         Game::TARGET_DIMENSIONS[Y] / 2 - btn_h / 2 }),
        math::Vector2i({ rl_btn_w, btn_h })
    );

    m_buttons[BUTTON_RIGHT] = std::make_unique<ui::Button>(
        *m_font,
        ">",
        math::Vector4f::one(),
        m_button_texture,
        math::Vector2i::zero(),
        m_button_texture.get_dimensions(),
        math::Vector2i({ Game::TARGET_DIMENSIONS[X] / 2 + rl_btn_offset,
                         Game::TARGET_DIMENSIONS[Y] / 2 - btn_h / 2}),
        math::Vector2i({ rl_btn_w, btn_h })
    );

    m_buttons[BUTTON_OK] = std::make_unique<ui::Button>(
        *m_small_font,
        "Ready",
        math::Vector4f::one(),
        m_button_texture,
        math::Vector2i::zero(),
        m_button_texture.get_dimensions(),
        math::Vector2i({ Game::TARGET_DIMENSIONS[X] / 2 - start_btn_w / 4, 60 }),
        math::Vector2i({ start_btn_w / 2, btn_h / 2 })
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

ServerLobbyState::~ServerLobbyState()
{
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

void ServerLobbyState::on_enter()
{
    open_socket();

    graphics::RenderTools::set_clear_color(0.0f, 0.25f, 0.5f, 1.0f);
    m_music.play();
}

void ServerLobbyState::on_exit()
{
    m_music.stop();
}

void ServerLobbyState::update(std::chrono::milliseconds const dt)
{
    process_udp_queue();

    m_name_input->update(dt, m_keyboard);

    auto all_ready = true;
    for (auto player : m_connections)
    {
        if (player->get_status() != network::Connection::Status::LOBBY_READY)
            all_ready = false;
    }

    for (int i = 0; i < M_NUM_BUTTONS; ++i)
    {
        auto& btn = m_buttons[i];

        bool const hovered = btn->is_point_inside(
            math::Vector2i(m_mouse.get_cursor_world_position(math::Vector2f::zero()))
        );
        if (hovered)
        {
            btn->get_sprite().set_color(math::Vector4f({ 0.0f, 0.0f, 0.0f, 1.0f }));
            if (!all_ready && i == BUTTON_START)
                btn->get_sprite().set_color(math::Vector4f::one());

            if (m_mouse.was_button_pressed(GLFW_MOUSE_BUTTON_1))
            {
                if (i == BUTTON_LEFT)
                {
                    m_selected_level = m_selected_level - 1 < 0 
                                     ? NUM_LEVELS - 1
                                     : m_selected_level - 1;
                    update_thumbnail();

                    // Make-shift level select packet using keepalive byte
                    auto level = std::to_string(m_selected_level + 1);
                    m_socket->broadcast(
                        network::UdpPacket(
                            network::UDP_H_KEEPALIVE | network::UDP_H_DATABLOCK,
                            const_cast<char*>(level.c_str())
                        )
                    );
                }
                else if (i == BUTTON_RIGHT)
                {
                    m_selected_level = m_selected_level + 1 > NUM_LEVELS - 1
                                     ? 0
                                     : m_selected_level + 1;
                    update_thumbnail();

                    auto level = std::to_string(m_selected_level + 1);
                    m_socket->broadcast(
                        network::UdpPacket(
                            network::UDP_H_KEEPALIVE | network::UDP_H_DATABLOCK,
                            const_cast<char*>(level.c_str())
                        )
                    );
                }
                else if (i == BUTTON_START)
                {
                    if (!all_ready)
                    {
                        break;
                    }

                    // Broadcast start command
                    m_socket->broadcast(
                        network::UdpPacket(
                            network::UDP_H_OK  |
                            network::UDP_H_KEEPALIVE
                        )
                    );
                    m_socket->end_listener_routine();
                    m_socket->close();
                    m_listener_thread.join();
                    m_game.host_game(LEVEL_IDS[m_selected_level],
                                     LEVEL_PATHS[m_selected_level],
                                     std::move(m_socket),
                                     m_connections);
                    break;
                }
                else if (i == BUTTON_OK)
                {
                    m_socket->broadcast(
                        network::UdpPacket(
                            network::UDP_H_OK  |
                            network::UDP_H_POS |
                            network::UDP_H_DATABLOCK,
                            m_id,
                            const_cast<char*>(m_name_input->get_value().c_str())
                        )
                    );
                    break;
                }
            }
        }
        else
        {
            btn->get_sprite().set_color(math::Vector4f::one());
            if (i == BUTTON_START && all_ready)
                btn->get_sprite().set_color(math::Vector4f({ 1.0f, 0.5f, 0.0f, 1.0f }));
        }
    }

    if (m_keyboard.was_key_pressed(GLFW_KEY_F4))
    {
        close_socket();
        m_game.pop_state();
        return;
    }
}

void ServerLobbyState::render()
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
        m_name_input->get_value(),
        m_connections_text_position - math::Vector2f({ 0.0f, 40.0f }),
        math::Vector2f({ 1.0f, -1.0f }),
        math::Vector4f({ 0.0f, 1.0f, 0.0f, 1.0f })
    );
    auto c_blue = 2;
    auto c_red  = 1;
    for (auto player : m_connections)
    {
        auto pos = m_connections_text_position;
        auto col = math::Vector4f::one();
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
            col = math::Vector4f({ 0.0f, 1.0f, 0.0f, 1.0f });
        }

        m_font_renderer.render_shadowed_text(
            *m_small_font,
            player->get_name(),
            pos,
            math::Vector2f({ 1.0f, -1.0f }),
            col
        );
    }
    m_font_renderer.render_shadowed_text(
        *m_small_font,
        "Server IP: " + m_socket->get_ip(),
        math::Vector2f({ 60.0f, 140.0f}),
        math::Vector2f({ 1.0f, -1.0f }),
        math::Vector4f::one()
    );

    m_name_input->render_text(m_font_renderer);
}

void ServerLobbyState::update_thumbnail()
{
    m_thumbnail_sprite->set_source_position(math::Vector2i({
        0, static_cast<int>(m_selected_level) * THUMBNAIL_SOURCE_DIMENSIONS[Y]
    }));

    float const txt_w = m_font->get_text_width(LEVEL_NAMES[m_selected_level]);
    m_thumbnail_text_position[X] = static_cast<float>(Game::TARGET_DIMENSIONS[X] / 2)
                                 - txt_w / 2.0f;
}

void ServerLobbyState::open_socket()
{
    m_listener_thread = std::thread([&]
    {
        m_socket->start_listener_routine();
    });
}

void ServerLobbyState::close_socket()
{
    m_socket->end_listener_routine();
    m_socket->close();
    m_listener_thread.join();
}

void ServerLobbyState::add_player(sockaddr_in client_address)
{
    for (auto p : m_connections)
    {
        if (p->get_sockaddr().sin_addr.S_un.S_addr ==
            client_address.sin_addr.S_un.S_addr)
        {
            // Deny players from the same address
            //return;
        }
    }
    auto new_player(std::make_shared<network::Connection>(client_address));
    assign_team(new_player);
    new_player->set_status(network::Connection::Status::CONNECTED);
    new_player->set_name("Player " + std::to_string(m_connections.size() + 2));

    // Send connected player unique and public ids
    m_socket->send(
        network::UdpPacket(
            network::UDP_H_CONNECT | network::UDP_H_OK | network::UDP_H_DATABLOCK | network::UDP_H_POS,
            new_player->get_public_id(),
            new_player->get_id()
        ), client_address
    );
    // Inform other players
    m_socket->broadcast(
        network::UdpPacket(
            network::UDP_H_CONNECT | network::UDP_H_POS | network:: UDP_H_DATABLOCK,
            new_player->get_public_id(),
            const_cast<char*>(new_player->get_name().c_str())
        )
    );

    printf("-- DEBUG -- Player %s (%c) added\n", new_player->get_id(), new_player->get_public_id());
    m_connections.push_back(new_player);
    m_socket->add_client(new_player);

    // Add host
    m_socket->send(
        network::UdpPacket(
            network::UDP_H_CONNECT | network::UDP_H_POS | network:: UDP_H_DATABLOCK,
            m_id,
            const_cast<char*>(m_name_input->get_value().c_str())
        ), client_address
    );
    // Send lobby players' data
    for (auto p : m_connections)
    {
        m_socket->send(
            network::UdpPacket(
                network::UDP_H_CONNECT | network::UDP_H_POS | network:: UDP_H_DATABLOCK,
                p->get_public_id(),
                const_cast<char*>(p->get_name().c_str())
            ), client_address
        );

        // Ready status
        if (p->get_status() != network::Connection::Status::LOBBY_READY)
            continue;

        m_socket->send(
            network::UdpPacket(
                network::UDP_H_OK | network::UDP_H_POS | network:: UDP_H_DATABLOCK,
                p->get_public_id(),
                const_cast<char*>(p->get_name().c_str())
            ), client_address
        );
    }
    // Send level + 1 for null reasons
    auto level = std::to_string(m_selected_level + 1);
    m_socket->send(
        network::UdpPacket(
            network::UDP_H_KEEPALIVE | network::UDP_H_DATABLOCK,
            const_cast<char*>(level.c_str())
        ), client_address
    );

}

void ServerLobbyState::assign_team(std::shared_ptr<network::Connection> player) const
{
    auto t1 = get_player_count(network::UDP_POS_TEAM_1);
    auto t2 = get_player_count(network::UDP_POS_TEAM_2);

    if (t1 == network::ROOM_MAX_USERS &&
        t2 == network::ROOM_MAX_USERS)
        return;

    auto team = network::UDP_POS_TEAM_1;
    if (t2 < t1)
        team = network::UDP_POS_TEAM_2;

    auto pos = get_open_position(team);
    player->team = team;
    player->player_num = pos;
}

void ServerLobbyState::drop_player(std::shared_ptr<network::Connection> player)
{
    m_socket->drop_client(player);
    m_connections.erase(remove(m_connections.begin(),
                               m_connections.end(),
                               player),
                               m_connections.end());
    m_socket->broadcast(network::UdpPacket(network::UDP_H_CONNECT |
                                          network::UDP_H_ERROR |
                                          network::UDP_H_POS,
                                          player->get_public_id()));
}

void ServerLobbyState::process_packet(network::UdpPacket packet, sockaddr_in client_address)
{
    if (packet.header_contains(network::UDP_H_CONNECT) &&
        packet.header_contains(network::UDP_H_ERROR) &&
        packet.header_contains(network::UDP_H_DATABLOCK))
    {
        for (auto player : m_connections)
        {
            if (strcmp(player->get_id(), packet.get_data_block()) != 0)
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

            if (player->get_status() == network::Connection::Status::CONNECTED)
                player->set_status(network::Connection::Status::LOBBY_READY);
            else
                player->set_status(network::Connection::Status::CONNECTED);
            player->set_name(packet.get_data_block());
            m_socket->broadcast(packet);
            break;
        }
    }
    if (packet.header_contains(network::UDP_H_CONNECT))
    {
        add_player(client_address);
        return;
    }
}

void ServerLobbyState::process_udp_queue()
{
    while (m_socket->has_packets())
    {
        auto pair = m_socket->get_packet();
        process_packet(pair.first, pair.second);
    }
}

unsigned char ServerLobbyState::get_open_position(unsigned char const team) const
{
    // TODO: fix stupid implementation
    auto pos1 = true;
    auto pos2 = true;
    auto pos3 = true;
    auto pos4 = true;
    for (auto player : m_connections)
    {
        if (!player.get()->team & team)
            continue;

        auto num = player.get()->player_num;
        if (num & network::UDP_POS_PLAYER_1 || team == network::UDP_POS_TEAM_1)
            pos1 = false;
        if (num & network::UDP_POS_PLAYER_2)
            pos2 = false;
        if (num & network::UDP_POS_PLAYER_3)
            pos3 = false;
        if (num & network::UDP_POS_PLAYER_4)
            pos4 = false;
    }
    if (pos1)
        return network::UDP_POS_PLAYER_1;
    if (pos2)
        return network::UDP_POS_PLAYER_2;
    if (pos3)
        return network::UDP_POS_PLAYER_3;
    if (pos4)
        return network::UDP_POS_PLAYER_4;

    return 0;
}

int ServerLobbyState::get_player_count(unsigned char const team) const
{
    auto count = 0;
    if (team == network::UDP_POS_TEAM_1)
        count = 1;
    for (auto player : m_connections)
    {
        if (player->team & team)
            count++;
    }
    return count;
}

} // namespace logic
