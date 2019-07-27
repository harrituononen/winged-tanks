#ifndef NETWORK_CONNECTION_HPP
#define NETWORK_CONNECTION_HPP

#include <thread>
#include <WinSock2.h>

#include "utilities/config.hpp"

namespace network {

class Connection
{
public:
    enum class Status
    {
        OFFLINE,
        CONNECTED,
        LOBBY_READY,
        WAITING,
        IN_GAME,
        DEAD
    };

protected:
    char            m_id[NETWORK_ID_LENGTH];
    sockaddr_in     m_sockaddr;
    time_t          m_last_successful_ping;
    std::string     m_name;
    Status          m_status;

public:
    unsigned char   player_num;
    unsigned char   team;
    int             consecutive_pings_missed;

    explicit Connection(sockaddr_in const si);
             Connection(Connection const& conn) = default;
             Connection(U8 const team, U8 const pos);
            ~Connection() = default;

    bool operator==(Connection const& rhs) const;

    bool                connection_lost         () const;
    std::string         get_id                  () const;
    char*               get_id                  ();
    int                 get_last_successful_ping() const;
    std::string         get_name                () const;
    U8                  get_public_id           () const;
    struct sockaddr_in  get_sockaddr            () const;
    Status              get_status              () const;

    void                ping_missed             ();
    void                ping_received           ();
    void                set_name                (std::string name);
    void                set_player_num          (U8 const pos);
    void                set_status              (Status st);
    void                set_team                (U8 const team);
};

} // namespace network

#endif // NETWORK_CONNECTION_HPP
