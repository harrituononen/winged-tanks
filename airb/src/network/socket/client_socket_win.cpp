#include "client_socket_win.hpp"

#include <cstdio>
#include <thread>

#include "../player.hpp"
#include "../utilities/functions.hpp"

namespace network {

ClientSocket::ClientSocket(std::string server_address, unsigned short server_port)
    : m_listener_running(false)
    , m_keepalive_running(false)
    , m_id { '\0' }
    , m_server_addr(server_address)
    , m_queue()
{
    memset(reinterpret_cast<char *>(&m_server_sockaddr), 0, sizeof m_server_sockaddr);
    m_server_sockaddr.sin_family = AF_INET;
    m_server_sockaddr.sin_port = htons(server_port);
    m_server_sockaddr.sin_addr.S_un.S_addr = inet_addr(m_server_addr.c_str());

    open();
}

ClientSocket::~ClientSocket()
{
    close();
}

void ClientSocket::open()
{
    WSAData wsa;

    // Initialise winsock
    print_time();
    printf("Initialising Winsock... ");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Failed. Error Code : %d\n", WSAGetLastError());
        throw std::runtime_error("WSAStartup() failed.");
    }
    printf("Initialised.\n");

    // Create socket
    m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_socket == INVALID_SOCKET)
    {
        print_time();
        printf("socket() failed with error code : %d\n", WSAGetLastError());
        throw std::runtime_error("Socket creation failed.");
    }

    struct sockaddr_in listener;
    listener.sin_family = AF_INET;
    listener.sin_port = htons(8887);
    listener.sin_addr.s_addr = INADDR_ANY;
    if (bind(m_socket, reinterpret_cast<struct sockaddr *>(&listener), sizeof listener) == SOCKET_ERROR)
    {
        printf("Bind failed with error code : %d\n", WSAGetLastError());
        throw std::runtime_error("Socket bind() failed.");
    }
}

void ClientSocket::close() const
{
    shutdown(m_socket, SD_BOTH);
    closesocket(m_socket);
    WSACleanup();
}

void ClientSocket::init_connect()
{
    print_time();
    printf("Attempting to connect...\n");
    send(UdpPacket(UDP_H_CONNECT));
}

void ClientSocket::start_listener_routine()
{
    char buffer[UDP_MAX_PACKET_SIZE];
    int  slen = sizeof m_server_sockaddr;
    int  recv_data;

    m_listener_running = true;
    while (m_listener_running)
    {
        memset(buffer, UDP_H_NULL, UDP_MAX_PACKET_SIZE);

        recv_data = recvfrom(m_socket,
                             buffer,
                             sizeof buffer,
                             0,
                             reinterpret_cast<struct sockaddr *>(&m_server_sockaddr),
                             &slen);
        if (recv_data == SOCKET_ERROR)
        {
            print_time();
            printf("recvfrom() failed with error code : %d\n", WSAGetLastError());
            std::this_thread::sleep_for(std::chrono::milliseconds(PING_FREQUENCY_MS));
            continue;
        }

        process_packet(UdpPacket(buffer));
    }
}

void ClientSocket::start_keepalive_routine()
{
    m_keepalive_running = true;
    while (m_keepalive_running)
    {
        while(m_id[0] == '\0')
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(PING_FREQUENCY_MS/4));
            // Debug window closed
            if (!m_keepalive_running)
                return;
        }

        send(UdpPacket(UDP_H_KEEPALIVE | UDP_H_DATABLOCK, m_id));
        std::this_thread::sleep_for(std::chrono::milliseconds(PING_FREQUENCY_MS));
    }
}

void ClientSocket::end_listener_routine()
{
    m_listener_running = false;
}

void ClientSocket::end_keepalive_routine()
{
    m_keepalive_running = false;
}

void ClientSocket::send(UdpPacket packet)
{
    int slen = sizeof m_server_sockaddr;

    if (sendto(m_socket,
               packet.to_char_array(),
               packet.get_size(),
               0,
               reinterpret_cast<struct sockaddr *>(&m_server_sockaddr),
               slen) == SOCKET_ERROR)
    {
        print_time();
        printf("sendto() failed with error code : %d\n", WSAGetLastError());
    }
    if (!packet.header_contains(UDP_H_KEEPALIVE) && !packet.header_contains(UDP_H_INPUT))
    {
        print_time();
        printf("Packet sent to server.\n");
        packet.print();
    }
}

void ClientSocket::set_client_id(char const* id)
{
    auto i = 0;
    while (id[i] != '\0')
        m_id[i] = id[i++];
}


bool ClientSocket::has_packets() const
{
    return !m_queue.empty();
}

char* ClientSocket::get_client_id() const
{
    return const_cast<char*>(&m_id[0]);
}

UdpPacket ClientSocket::get_packet()
{
    return m_queue.dequeue();
}

std::string ClientSocket::get_server_addr() const
{
    return m_server_addr;
}

void ClientSocket::process_packet(UdpPacket const packet)
{
    if (!packet.header_contains(UDP_H_INPUT) && !packet.header_contains(UDP_H_POS))
    {
        print_time();
        printf("Received packet from server\n");
        packet.print();
    }
    if (packet.header_contains(UDP_H_CONNECT) &&
        packet.header_contains(UDP_H_OK) &&
        packet.header_contains(UDP_H_DATABLOCK))
    {
        // First connect, server returns unique id
        strcpy_s(m_id, packet.get_data_block());
        print_time();
        printf("Connected. My id is %s\n", m_id);
    }
    if (packet.header_contains(UDP_H_CONNECT) &&
        packet.header_contains(UDP_H_ERROR))
    {
        print_time();
        printf("Server refused connection...\n");
        //close();
    }
    m_queue.enqueue(packet);
}

} // namespace network
