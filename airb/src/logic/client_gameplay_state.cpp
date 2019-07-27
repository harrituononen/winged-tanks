#include "client_gameplay_state.hpp"

#include "game.hpp"

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

ClientGameplayState::ClientGameplayState(Game& g,
                                         graphics::Textures const lvl_id,
                                         std::unique_ptr<network::ClientSocket> socket,
                                         unsigned char const public_id,
                                         std::vector<std::shared_ptr<network::Connection>> players)
    : GameplayState(g, lvl_id)
    , m_cursor_sync_running(false)
    , m_host_quit(false)
    , m_waiting_for_players(true)
    , m_public_id(public_id)
    , m_socket(std::move(socket))
{
    for (auto p : players)
    {
        add_player(*p.get());
    }

    open_socket();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    m_socket->send(network::UdpPacket(network::UDP_H_CONNECT | network::UDP_H_DATABLOCK, m_socket->get_client_id()));
    while (m_waiting_for_players)
    {
        process_udp_queue();

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

ClientGameplayState::~ClientGameplayState()
{
    m_socket->send(network::UdpPacket(
        network::UDP_H_CONNECT |
        network::UDP_H_ERROR   |
        network::UDP_H_DATABLOCK,
        m_socket->get_client_id())
    );
    close_socket();
}

void ClientGameplayState::on_enter()
{
    GameplayState::on_enter();
}

void ClientGameplayState::on_exit()
{
    GameplayState::on_exit();
}

void ClientGameplayState::update(std::chrono::milliseconds const dt)
{
    process_udp_queue();

    // Host sent kill byte
    if (m_host_quit)
    {
        m_game.pop_state();
        return;
    }

    update_released_keys();
    update_pressed_keys();

    GameplayState::update(dt);
}

void ClientGameplayState::render()
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

void ClientGameplayState::update_player(std::chrono::milliseconds const dt)
{
    GameplayState::update_player(dt);

    for (auto p : m_players)
    {
        p->update(dt, m_terrain);
    }
}

void ClientGameplayState::open_socket()
{
    m_socket->open();
    m_listener_thread = std::thread([&]
    {
        m_socket->start_listener_routine();
    });
    m_keepalive_thread = std::thread([&]
    {
        m_socket->start_keepalive_routine();
    });
    m_cursor_sync_thread = std::thread([&]
    {
        start_cursor_sync_routine();
    });
}

void ClientGameplayState::close_socket()
{
    end_cursor_sync_routine();
    m_socket->end_listener_routine();
    m_socket->end_keepalive_routine();
    m_socket->close();
    m_cursor_sync_thread.join();
    m_listener_thread.join();
    m_keepalive_thread.join();
}

void ClientGameplayState::add_player(network::Connection conn)
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
    m_players.push_back(move(new_player));
}

void ClientGameplayState::drop_player(std::shared_ptr<network::Player> player)
{
    m_players.erase(remove(m_players.begin(),
                           m_players.end(),
                           player),
                           m_players.end());
}

void ClientGameplayState::process_packet(network::UdpPacket packet)
{
    if (packet.header_contains(network::UDP_H_CONNECT) &&
        packet.header_contains(network::UDP_H_ERROR) &&
        packet.header_contains(network::UDP_H_POS))
    {
        for (auto player : m_players)
        {
            if (packet.position_is(network::UDP_POS_TEAM_1 | network::UDP_POS_PLAYER_1))
            {
                m_host_quit = true;
                return;
            }

            if (!packet.position_is(player->get_public_id()))
                continue;

            drop_player(player);
            return;
        }
        return;
    }
    if (packet.header_contains(network::UDP_H_CONNECT | network::UDP_H_OK))
    {
        m_waiting_for_players = false;
        return;
    }
    // Cursor sync
    if (packet.header_contains(network::UDP_H_INPUT) &&
        packet.header_contains(network::UDP_H_POS) &&
        packet.header_contains(network::UDP_H_DATABLOCK))
    {
        if (m_players.size() == 0)
            return;

        char buffer[network::UDP_MAX_DATABLOCK_SIZE] = { '\0' };
        auto data = packet.get_data_block();

        auto id = data[0];

        // cursor x
        auto byte = 1;
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
            if (player->get_public_id() != id)
                continue;

            player->set_cursor_position(x, y);
            break;
        }
        return;
    }
    if (packet.header_contains(network::UDP_H_INPUT))
    {
        for (auto player : m_players)
        {
            if (!packet.position_is(player->get_public_id()))
                continue;

            player->process_packet(packet);
            break;
        }
        return;
    }
    if (packet.header_contains(network::UDP_H_POS))
    {
        if (packet.position_is(m_public_id))
        {
            m_player.process_packet(packet);
            return;
        }
        for (auto player : m_players)
        {
            if (!packet.position_is(player->get_public_id()))
                continue;

            player->process_packet(packet);
            break;
        }
        return;
    }
}

void ClientGameplayState::process_udp_queue()
{
    while (m_socket->has_packets())
    {
        process_packet(m_socket->get_packet());
    }
}

void ClientGameplayState::update_pressed_keys()
{
    // TODO: Should we check !is_key_held() ??
    auto move   = network::UDP_H_NULL;
    auto action = network::UDP_H_NULL;
    if (m_window.keyboard.was_key_pressed(input::KEY_THRUST))
        move |= network::UDP_IN_UP;
    if (m_window.keyboard.was_key_pressed(input::KEY_DECELERATE))
        move |= network::UDP_IN_DOWN;
    if (m_window.keyboard.was_key_pressed(input::KEY_YAW_LEFT))
        move |= network::UDP_IN_LEFT;
    if (m_window.keyboard.was_key_pressed(input::KEY_YAW_RIGHT))
        move |= network::UDP_IN_RIGHT;

    if (m_window.mouse.was_button_pressed(input::BUTTON_FIRE))
        action |= network::UDP_IN_SHOOT;
    if (m_window.keyboard.was_key_pressed(input::KEY_RELOAD))
        action |= network::UDP_IN_RELOAD;
    if (m_window.keyboard.was_key_pressed(input::KEY_WEAPON_1))
        action |= network::UDP_IN_SWITCH_WPN_1;
    if (m_window.keyboard.was_key_pressed(input::KEY_WEAPON_2))
        action |= network::UDP_IN_SWITCH_WPN_2;
    if (m_window.keyboard.was_key_pressed(input::KEY_WEAPON_3))
        action |= network::UDP_IN_SWITCH_WPN_3;
    if (m_window.keyboard.was_key_pressed(input::KEY_WEAPON_PREVIOUS))
        action |= network::UDP_IN_SWITCH_WPN_Q;

    if (move   != network::UDP_H_NULL ||
        action != network::UDP_H_NULL)
        m_socket->send(network::UdpPacket(network::UDP_H_INPUT  | network::UDP_H_DATABLOCK,
                                         network::UDP_IN_PRESS | move,
                                         network::UDP_IN_PRESS | action,
                                         m_socket->get_client_id()));
}

void ClientGameplayState::update_released_keys()
{
    auto move   = network::UDP_H_NULL;
    auto action = network::UDP_H_NULL;
    if (m_window.keyboard.was_key_released(input::KEY_THRUST))
        move |= network::UDP_IN_UP;
    if (m_window.keyboard.was_key_released(input::KEY_DECELERATE))
        move |= network::UDP_IN_DOWN;
    if (m_window.keyboard.was_key_released(input::KEY_YAW_LEFT))
        move |= network::UDP_IN_LEFT;
    if (m_window.keyboard.was_key_released(input::KEY_YAW_RIGHT))
        move |= network::UDP_IN_RIGHT;

    if (m_window.mouse.was_button_released(input::BUTTON_FIRE))
        action |= network::UDP_IN_SHOOT;

    if (move != network::UDP_H_NULL ||
        action != network::UDP_H_NULL)
        m_socket->send(network::UdpPacket(network::UDP_H_INPUT | network::UDP_H_DATABLOCK,
                                         move,
                                         action,
                                         m_socket->get_client_id()));
}

void ClientGameplayState::start_cursor_sync_routine()
{
    m_cursor_sync_running = true;
    while (m_cursor_sync_running)
    {
        while (m_socket->get_client_id()[0] == '\0')
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(network::PING_FREQUENCY_MS / 4));
            if (!m_cursor_sync_running)
                return;
        }

        char buffer[network::UDP_MAX_DATABLOCK_SIZE];
        sprintf_s(buffer, "%s|%f|%f",
                  m_socket->get_client_id(),
                  m_mouse_world_position[X],
                  m_mouse_world_position[Y]);
        m_socket->send(network::UdpPacket(
                      network::UDP_H_INPUT | network::UDP_H_POS | network::UDP_H_DATABLOCK,
                      buffer));

        std::this_thread::sleep_for(std::chrono::milliseconds(network::CURSOR_SYNC_FREQUENCY_MS));
    }
}

void ClientGameplayState::end_cursor_sync_routine()
{
    m_cursor_sync_running = false;
}

} // namespace logic
