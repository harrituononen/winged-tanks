#ifndef NETWORK_CLIENT_SOCKET_WIN_HPP
#define NETWORK_CLIENT_SOCKET_WIN_HPP

#include <memory>
#include <winsock2.h>

#pragma comment(lib,"ws2_32.lib")

#include "../utilities/udp_packet.hpp"
#include "../../utilities/async_queue.hpp"

namespace network {

class ClientSocket
{
private:
    bool        m_listener_running;
    bool        m_keepalive_running;
    char        m_id[NETWORK_ID_LENGTH];
    SOCKET      m_socket;
    std::string m_server_addr;
    sockaddr_in m_server_sockaddr;

    utilities::AsyncQueue<UdpPacket> m_queue;

public:
     ClientSocket(std::string server_address, unsigned short server_port);
    ~ClientSocket();

    void open                   ();
    void close                  () const;
    void init_connect           ();
    void send                   (UdpPacket packet);
    void set_client_id          (char const* id);

    void start_listener_routine ();
    void start_keepalive_routine();
    void end_listener_routine   ();
    void end_keepalive_routine  ();

    bool        has_packets     () const;
    char*       get_client_id   () const;
    UdpPacket   get_packet      ();
    std::string get_server_addr () const;

private:
    void process_packet(UdpPacket const packet);

};

} // namespace network

#endif // NETWORK_CLIENT_SOCKET_WIN_HPP
