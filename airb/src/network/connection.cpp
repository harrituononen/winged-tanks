#include "connection.hpp"

#pragma warning(disable : 4996)

#include <ctime>

#include "../network/utilities/udp_packet.hpp"

namespace network {

Connection::Connection(sockaddr_in const si)
    : m_sockaddr(si)
    , m_last_successful_ping(time(nullptr))
    , m_name("")
    , player_num(0)
    , team(0)
    , consecutive_pings_missed(0)
    , m_status(Status::OFFLINE)
{
    // Dummy id implementation
    memset(m_id, NETWORK_ID_LENGTH, '\0');
    snprintf(m_id,
             NETWORK_ID_LENGTH,
             "%s-%d",
             inet_ntoa(si.sin_addr),
             static_cast<int>(time(nullptr)));
}

Connection::Connection(U8 const t, U8 const pos)
    : Connection(sockaddr_in())
{
    set_team(t);
    set_player_num(pos);
}

bool Connection::operator==(Connection const& rhs) const
{
    return strcmp(m_id, rhs.m_id) == 0;
}

bool Connection::connection_lost() const
{
    return consecutive_pings_missed > MISSED_PINGS_ALLOWED;
}

std::string Connection::get_id() const
{
    return m_id;
}

char* Connection::get_id()
{
    return const_cast<char*>(m_id);
}

U8 Connection::get_public_id() const
{
    return team | player_num;
}

int Connection::get_last_successful_ping() const
{
    return static_cast<int>(m_last_successful_ping);
}

std::string Connection::get_name() const
{
    return m_name;
}

sockaddr_in Connection::get_sockaddr() const
{
    return m_sockaddr;
}

Connection::Status Connection::get_status() const
{
    return m_status;
}

void Connection::ping_missed()
{
    consecutive_pings_missed++;
}

void Connection::ping_received()
{
    m_last_successful_ping = time(nullptr);
    consecutive_pings_missed = 0;
}

void Connection::set_name(std::string name)
{
    m_name = name;
}

void Connection::set_player_num(U8 const pos)
{
     player_num = pos;
}

void Connection::set_status(Status st)
{
    m_status = st;
}

void Connection::set_team(U8 const t)
{
    team = t;
}

} // namespace network
