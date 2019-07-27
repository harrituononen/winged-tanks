#include "server_socket_win.hpp"

#include <stdio.h>

#include "../utilities/config.hpp"
#include "../utilities/functions.hpp"

namespace network {

ServerSocket::ServerSocket(unsigned short port)
    : m_listener_running(false)
    , m_conn_check_running(false)
    , m_port(port)
    , m_connections()
    , m_queue()
{
    m_sockaddr.sin_family = AF_INET;
    m_sockaddr.sin_port = htons(m_port);
    m_sockaddr.sin_addr.s_addr = INADDR_ANY;

    open();
}

ServerSocket::~ServerSocket()
{
    close();
}

void ServerSocket::open()
{
    WSAData wsa;

    // Initialise winsock
    print_time();
    printf("Initialising Winsock...");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf(" Failed. Error Code : %d\n", WSAGetLastError());
        throw std::runtime_error("WSAStartup() failed.");
    }
    printf(" Initialised.\n");

    // Create a socket
    print_time();
    m_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_socket == INVALID_SOCKET)
    {
        printf("Could not create socket : %d\n", WSAGetLastError());
        throw std::runtime_error("Socket creation failed.");
    }
    printf("Socket created.\n");

    // Bind
    print_time();
    if (bind(m_socket, reinterpret_cast<struct sockaddr *>(&m_sockaddr), sizeof m_sockaddr) == SOCKET_ERROR)
    {
        printf("Bind failed with error code : %d\n", WSAGetLastError());
        throw std::runtime_error("Socket bind() failed.");
    }
    printf("Socket bind successful.\n");
}

void ServerSocket::process_packet(UdpPacket const packet, sockaddr_in const si_client)
{
    if (!packet.header_contains(UDP_H_KEEPALIVE) && !packet.header_contains(UDP_H_INPUT))
    {
        // Debug
        print_time();
        printf("Received packet from %s:%d\n",
               inet_ntoa(si_client.sin_addr),
               ntohs(si_client.sin_port));
        printf("           > Packet size: %d byte(s)\n", packet.get_size());

        packet.print();
    }
    if (packet.header_contains(UDP_H_KEEPALIVE))
    {
        if (packet.get_data_block()[0] != '\0')
        {
            char id[NETWORK_ID_LENGTH] = { '\0' };
            strcpy_s(id, NETWORK_ID_LENGTH, packet.get_data_block());
            ping_received(id);
        }
        return;
    }
    m_queue.enqueue(std::pair<UdpPacket, sockaddr_in>(packet, si_client));
}

void ServerSocket::add_client(std::shared_ptr<Connection> client)
{
    m_connections.push_back(client);
}

void ServerSocket::broadcast(UdpPacket const packet) const
{
    for (auto const client : m_connections)
    {
        send(packet, client->get_sockaddr());
    }
}

void ServerSocket::broadcast(UdpPacket const packet, U8 const exclude_client_id) const
{
    for (auto const client : m_connections)
    {
        if (client->get_public_id() == exclude_client_id)
            continue;

        send(packet, client->get_sockaddr());
    }
}

void ServerSocket::drop_client(std::shared_ptr<Connection> client)
{
    print_time();
    printf("Client %s disconnected\n", client->get_id());
    for (auto c : m_connections)
    {
        if (strcmp(c->get_id(), client->get_id()) != 0)
            continue;

        m_connections.erase(remove(m_connections.begin(),
                                   m_connections.end(),
                                   c),
                                   m_connections.end());
        return;
    }
}

void ServerSocket::ping_received(char const* client_id) const
{
    for (auto const client : m_connections)
    {
        if (strcmp(client->get_id(), client_id) == 0)
        {
            client->ping_received();
        }
    }
}

void ServerSocket::send(UdpPacket packet, sockaddr_in to) const
{
    int slen = sizeof to;

    auto send_data = sendto(m_socket,
                            packet.to_char_array(),
                            packet.get_size(),
                            0,
                            reinterpret_cast<struct sockaddr*>(&to),
                            slen);
    if (send_data == SOCKET_ERROR)
    {
        print_time();
        printf("sendto() failed with error code : %d\n", WSAGetLastError());
    }
}

void ServerSocket::start_conn_check_routine()
{
    m_conn_check_running = true;
    while (m_conn_check_running)
    {
        if (m_connections.size() == 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(SYNC_FREQUENCY_MS));
            continue;
        }
        auto now = time(nullptr);
        for (auto const client : m_connections)
        {
            if (client->connection_lost())
                continue;

            // TODO: fix arbitrary constant value
            // Idea: Allow client accepting pings successfully with a slight time buffer
            if (now - client->get_last_successful_ping() > PING_FREQUENCY_MS * 1.9f / 1000)
            {
                client->ping_missed();
                if (client->connection_lost())
                {
                    print_time();
                    printf("Client %s missed %d consecutive pings. They may be disconnected.\n",
                            client->get_id(),
                            client->consecutive_pings_missed);
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(PING_FREQUENCY_MS));
    }
}

void ServerSocket::start_listener_routine()
{
    struct sockaddr_in si_client;
    int    recv_data;
    int    slen = sizeof si_client;
    char   buffer[UDP_MAX_PACKET_SIZE];

    print_time();
    printf("Waiting for data...\n");

    m_listener_running = true;
    while (m_listener_running)
    {
        memset(buffer, UDP_H_NULL, UDP_MAX_PACKET_SIZE);
        recv_data = recvfrom(m_socket,
                             buffer,
                             UDP_MAX_PACKET_SIZE,
                             0,
                             reinterpret_cast<struct sockaddr *>(&si_client),
                             &slen);
        if (recv_data == SOCKET_ERROR)
        {
            //print_time();
            //printf("recvfrom() failed with error code : %d\n", WSAGetLastError());
            std::this_thread::sleep_for(std::chrono::milliseconds(SYNC_FREQUENCY_MS));
            continue;
        }

        process_packet(UdpPacket(buffer), si_client);
    }
}

void ServerSocket::end_conn_check_routine()
{
    m_conn_check_running = false;
}

void ServerSocket::end_listener_routine()
{
    m_listener_running = false;
}

void ServerSocket::close() const
{
    shutdown(m_socket, SD_BOTH);
    closesocket(m_socket);
    WSACleanup();
}

bool ServerSocket::has_packets() const
{
    return !m_queue.empty();
}

std::pair<UdpPacket, sockaddr_in> ServerSocket::get_packet()
{
    return m_queue.dequeue();
}

std::string ServerSocket::get_ip()
{
    char ac[80];
    if (gethostname(ac, sizeof(ac)) == SOCKET_ERROR)
    {
        printf("Error getting host ip. Error Code : %d\n", WSAGetLastError());
        throw std::runtime_error("get_ip() gethostname() failed.");
    }

    struct hostent* phe = gethostbyname(ac);
    if (phe == 0)
    {
        throw std::runtime_error("get_ip() bad host lookup failed.");
    }

    struct in_addr addr;
    memcpy(&addr, phe->h_addr_list[0], sizeof(struct in_addr));
    return inet_ntoa(addr);
}

} // namespace network
