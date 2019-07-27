#include "server_gameplay_state.hpp"

#include "game.hpp"

namespace logic {

ServerGameplayState::ServerGameplayState(Game& g,
                                         graphics::Textures const lvl_id,
                                         std::unique_ptr<network::ServerSocket>& socket,
                                         std::vector<std::shared_ptr<network::Connection>> players)
    : GameplayState(g, lvl_id)
    , m_pos_sync_running(false)
    , m_cursor_sync_running(false)
    , m_public_id(network::UDP_POS_TEAM_1 | network::UDP_POS_PLAYER_1)
    , m_waiting_for_players(true)
    , m_socket(std::move(socket))
{
    open_socket();

    for (auto p : players)
    {
        add_player(*p.get());
    }

    if (m_players.size() == 0)
        return;

    while (m_waiting_for_players)
    {
        process_udp_queue();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    m_socket->broadcast(network::UdpPacket(network::UDP_H_CONNECT | network::UDP_H_OK));
}

ServerGameplayState::~ServerGameplayState()
{
    m_socket->broadcast(network::UdpPacket(
        network::UDP_H_CONNECT |
        network::UDP_H_ERROR   |
        network::UDP_H_POS,
        m_public_id)
    );
    close_socket();
}

void ServerGameplayState::on_enter()
{
    GameplayState::on_enter();
}

void ServerGameplayState::on_exit()
{
    GameplayState::on_exit();
}

void ServerGameplayState::update(std::chrono::milliseconds const dt)
{
    process_udp_queue();

    update_released_keys();
    update_pressed_keys();

    GameplayState::update(dt);
}

void ServerGameplayState::render()
{
    GameplayState::render();

    m_batch_renderer.begin();
    for (auto p : m_players)
    {
        m_batch_renderer.submit(p->get_sprite());
        m_batch_renderer.submit(p->get_weapon_sprite());
    }
    m_batch_renderer.end();
    m_batch_renderer.flush();
}

void ServerGameplayState::update_player(std::chrono::milliseconds const dt)
{
    GameplayState::update_player(dt);

    for (auto p : m_players)
    {
        p->update(dt, m_terrain);
    }
}

void ServerGameplayState::open_socket()
{
    m_socket->open();
    m_listener_thread = std::thread([&]
    {
        m_socket->start_listener_routine();
    });
    m_conn_check_thread = std::thread([&]
    {
        m_socket->start_conn_check_routine();
    });
    m_pos_sync_thread = std::thread([&]
    {
        start_pos_sync_routine();
    });
    m_cursor_sync_thread = std::thread([&]
    {
        start_cursor_sync_routine();
    });
}

void ServerGameplayState::close_socket()
{
    m_socket->end_listener_routine();
    m_socket->end_conn_check_routine();
    end_pos_sync_routine();
    end_cursor_sync_routine();
    m_socket->close();
    m_listener_thread.join();
    m_conn_check_thread.join();
    m_pos_sync_thread.join();
    m_cursor_sync_thread.join();
}

void ServerGameplayState::add_player(network::Connection conn)
{
    auto new_player(std::make_shared<network::Player>(conn));

    auto plane = std::make_unique<entities::UncontrollablePlane>(
        1.0f, 50000.0f, 1.75f,
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
        math::Vector2f::one() * 900.0f //spawn_point
    );
    plane->set_game_state(this);
    new_player->set_plane(std::move(plane));
    new_player->set_status(network::Connection::Status::WAITING);
    m_players.push_back(new_player);
}

void ServerGameplayState::drop_player(std::shared_ptr<network::Player> player)
{
    m_socket->drop_client(player);
    m_players.erase(remove(m_players.begin(),
                           m_players.end(),
                           player),
                           m_players.end());
    m_socket->broadcast(network::UdpPacket(
        network::UDP_H_CONNECT |
        network::UDP_H_ERROR   |
        network::UDP_H_POS,
        player->get_public_id())
    );
}

void ServerGameplayState::process_packet(network::UdpPacket packet, sockaddr_in client_address)
{
    (void)client_address;

    if (packet.header_contains(network::UDP_H_CONNECT) &&
        packet.header_contains(network::UDP_H_ERROR) &&
        packet.header_contains(network::UDP_H_DATABLOCK))
    {
        for (auto player : m_players)
        {
            if (strcmp(player->get_id(), packet.get_data_block()) != 0)
                continue;

            drop_player(player);
            return;
        }
        return;
    }
    if (packet.header_contains(network::UDP_H_CONNECT) &&
        packet.header_contains(network::UDP_H_DATABLOCK))
    {
        for (auto player : m_players)
        {
            if (strcmp(player->get_id(), packet.get_data_block()) != 0)
                continue;

            player->set_status(network::Connection::Status::IN_GAME);
        }

        auto all_ready = true;
        for (auto player : m_players)
        {
            if (player->get_status() != network::Connection::Status::IN_GAME)
            {
                all_ready = false;
                break;
            }
        }
        m_waiting_for_players = !all_ready;
        return;
    }
    if (packet.header_contains(network::UDP_H_INPUT) &&
        packet.header_contains(network::UDP_H_POS) &&
        packet.header_contains(network::UDP_H_DATABLOCK))
    {
        // Cursor sync
        if (m_players.size() == 0)
            return;

        char id[network::NETWORK_ID_LENGTH] = { '\0' };
        char buffer[network::UDP_MAX_DATABLOCK_SIZE] = { '\0' };
        auto data = packet.get_data_block();

        // id
        auto byte = 0;
        for (auto i = 0; data[byte] != '|'; i++, byte++)
            id[i] = data[byte];

        // cursor x
        byte++;
        for (auto i = 0; data[byte] != '|'; i++, byte++)
            buffer[i] = data[byte];
        auto x = static_cast<float>(atof(buffer));

        // cursor y
        byte++;
        for (auto e = 0; buffer[e] != '\0'; e++) buffer[e] = '\0';
        for (auto i = 0; data[byte] != '\0'; i++, byte++)
            buffer[i] = data[byte];
        auto y = static_cast<float>(atof(buffer));

        for (auto player : m_players)
        {
            if (strcmp(player->get_id(), id) != 0)
                continue;

            player->set_cursor_position(x, y);
            break;
        }
        return;
    }
    if (packet.header_contains(network::UDP_H_INPUT))
    {
        char id[network::NETWORK_ID_LENGTH] = { '\0' };
        strcpy_s(id, network::NETWORK_ID_LENGTH, packet.get_data_block());

        for (auto player : m_players)
        {
            if (strcmp(player->get_id(), id) != 0)
                continue;

            player->process_packet(packet);

            // Broadcast input to other players
            m_socket->broadcast(
                network::UdpPacket(
                    network::UDP_H_INPUT | network::UDP_H_POS,
                    packet.get_move_byte(),
                    packet.get_action_byte(),
                    player->get_public_id()
                 ), player->get_public_id()
            );
            break;
        }
        return;
    }
}

void ServerGameplayState::process_udp_queue()
{
    while (m_socket->has_packets())
    {
        auto pair = m_socket->get_packet();
        process_packet(pair.first, pair.second);
    }
}

void ServerGameplayState::update_pressed_keys() const
{
    auto input = network::UDP_H_NULL;
    if (m_window.mouse.was_button_pressed(input::BUTTON_FIRE))
        input |= network::UDP_IN_SHOOT;
    if (m_window.keyboard.was_key_pressed(input::KEY_RELOAD))
        input |= network::UDP_IN_RELOAD;
    if (m_window.keyboard.was_key_pressed(input::KEY_WEAPON_1))
        input |= network::UDP_IN_SWITCH_WPN_1;
    if (m_window.keyboard.was_key_pressed(input::KEY_WEAPON_2))
        input |= network::UDP_IN_SWITCH_WPN_2;
    if (m_window.keyboard.was_key_pressed(input::KEY_WEAPON_3))
        input |= network::UDP_IN_SWITCH_WPN_3;
    if (m_window.keyboard.was_key_pressed(input::KEY_WEAPON_PREVIOUS))
        input |= network::UDP_IN_SWITCH_WPN_Q;

    if (input != network::UDP_H_NULL)
        m_socket->broadcast(network::UdpPacket(
                           network::UDP_H_INPUT | network::UDP_H_POS,
                           network::UDP_H_NULL,
                           network::UDP_IN_PRESS | input,
                           m_public_id));
}

void ServerGameplayState::update_released_keys() const
{
    auto input = network::UDP_H_NULL;
    if (m_window.mouse.was_button_released(input::BUTTON_FIRE))
        input |= network::UDP_IN_SHOOT;
    
    if (input != network::UDP_H_NULL)
        m_socket->broadcast(network::UdpPacket(
                           network::UDP_H_INPUT | network::UDP_H_POS,
                           network::UDP_H_NULL,
                           input,
                           m_public_id));
}

void ServerGameplayState::start_pos_sync_routine()
{
    char buffer[network::UDP_MAX_DATABLOCK_SIZE];
    math::Vector2f pos;
    math::Vector2f vel;
    float rot;

    m_pos_sync_running = true;
    while (m_pos_sync_running)
    {
        if (m_players.size() == 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(network::SYNC_FREQUENCY_MS));
            continue;
        }
        for (auto p : m_players)
        {
            memset(buffer, network::UDP_H_NULL, network::UDP_MAX_DATABLOCK_SIZE);
            pos = p->get_position();
            vel = p->get_velocity();
            rot = p->get_rotation();
            sprintf_s(buffer, "%f|%f|%f|%f|%f",
                      pos[X], pos[Y],
                      rot,
                      vel[X], vel[Y]/*,
                      p->get_weapon_num(),
                      p->get_weapon_ammo()*/);
            m_socket->broadcast(network::UdpPacket(
                               network::UDP_H_POS | network::UDP_H_DATABLOCK,
                               p->get_public_id(),
                               buffer));

            if (p->connection_lost())
            {
                drop_player(p);
                break;
            }
        }
        // Sync host plane
        {
            memset(buffer, network::UDP_H_NULL, network::UDP_MAX_DATABLOCK_SIZE);
            pos = m_player.get_rigid_body().get_position();
            vel = m_player.get_rigid_body().get_velocity();
            rot = m_player.get_rigid_body().get_rotation();
            sprintf_s(buffer, "%f|%f|%f|%f|%f",
                      pos[X], pos[Y],
                      rot,
                      vel[X], vel[Y]/*,
                      m_player.current_weapon,
                      m_player.get_weapon().get_rounds()*/);
            m_socket->broadcast(network::UdpPacket(
                               network::UDP_H_POS | network::UDP_H_DATABLOCK,
                               m_public_id,
                               buffer));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(network::SYNC_FREQUENCY_MS));
    }
}

void ServerGameplayState::start_cursor_sync_routine()
{
    m_cursor_sync_running = true;
    while (m_cursor_sync_running)
    {
        while (m_players.empty())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(network::PING_FREQUENCY_MS / 4));
            if (!m_cursor_sync_running)
                return;
        }

        char buffer[network::UDP_MAX_DATABLOCK_SIZE];
        for (auto p : m_players)
        {
            memset(buffer, network::UDP_MAX_DATABLOCK_SIZE, '\0');
            sprintf_s(buffer, "%c%f|%f",
                      p->get_public_id(),
                      p->get_cursor_position()[X],
                      p->get_cursor_position()[Y]);
            m_socket->broadcast(
                network::UdpPacket(
                    network::UDP_H_INPUT | network::UDP_H_POS | network::UDP_H_DATABLOCK,
                    buffer
                ), p->get_public_id()
            );
        }
        {
            // host cursor
            memset(buffer, network::UDP_MAX_DATABLOCK_SIZE, '\0');
            sprintf_s(buffer, "%c%f|%f",
                      m_public_id,
                      m_mouse_world_position[X],
                      m_mouse_world_position[Y]);
            m_socket->broadcast(
                network::UdpPacket(
                    network::UDP_H_INPUT | network::UDP_H_POS | network::UDP_H_DATABLOCK,
                    buffer
                )
            );
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(network::SYNC_FREQUENCY_MS));
    }
}

void ServerGameplayState::end_pos_sync_routine()
{
    m_pos_sync_running = false;
}

void ServerGameplayState::end_cursor_sync_routine()
{
    m_cursor_sync_running = false;
}

} // namespace logic
