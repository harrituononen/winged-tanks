#ifndef NETWORK_SERVER_SOCKET_WIN_HPP
#define NETWORK_SERVER_SOCKET_WIN_HPP

#include <vector>
#include <winsock2.h>

#pragma comment(lib,"ws2_32.lib")

#include "../connection.hpp"
#include "../utilities/udp_packet.hpp"
#include "../../utilities/async_queue.hpp"

namespace network {

class ServerSocket
{
    bool           m_listener_running;
    bool           m_conn_check_running;
    SOCKET         m_socket;
    sockaddr_in    m_sockaddr;
    unsigned short m_port;

    std::vector<std::shared_ptr<Connection>> m_connections;

    utilities::AsyncQueue<std::pair<UdpPacket, sockaddr_in>> m_queue;

    void process_packet(UdpPacket const packet, sockaddr_in const si_client);

public:
    explicit ServerSocket(unsigned short port);
            ~ServerSocket();

    void open                    ();
    void close                   () const;
    void add_client              (std::shared_ptr<Connection> client);
    void broadcast               (UdpPacket const packet)                             const;
    void broadcast               (UdpPacket const packet, U8 const exclude_client_id) const;
    void drop_client             (std::shared_ptr<Connection> client);
    void ping_received           (char const* client_id)                              const;
    void send                    (UdpPacket const packet, sockaddr_in const to)       const;

    void start_conn_check_routine();
    void start_listener_routine  ();
    void end_conn_check_routine  ();
    void end_listener_routine    ();

    bool                              has_packets() const;
    std::pair<UdpPacket, sockaddr_in> get_packet();

    std::string                       get_ip();
};

} // namespace network

#endif // NETWORK_SERVER_SOCKET_WIN_HPP
