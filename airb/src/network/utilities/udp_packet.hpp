#ifndef NETWORK_UTILITIES_UDP_PACKET
#define NETWORK_UTILITIES_UDP_PACKET

#include "config.hpp"

namespace network {

class UdpPacket
{
private:
    U8   m_header_byte_i = 0;
    U8   m_move_byte_i   = 1;
    U8   m_action_byte_i = 2;
    U8   m_pos_byte_i    = 3;
    U8   m_datablock_i   = 4;
    char m_packet[UDP_MAX_PACKET_SIZE];
public:
    UdpPacket();
    explicit UdpPacket(char buffer[]);
    explicit UdpPacket(U8 const header);
    UdpPacket(U8 const header, char data[]);
    UdpPacket(U8 const header, U8 const pos);
    UdpPacket(U8 const header, U8 const pos, char data[]);
    UdpPacket(U8 const header, U8 const input_move, U8 const input_action, char data[]);
    UdpPacket(U8 const header, U8 const input_move, U8 const input_action, U8 const pos);
    UdpPacket(U8 const header, U8 const input_move, U8 const input_action, U8 const pos, char data[]);
    ~UdpPacket();

    U8          get_header_byte () const;
    U8          get_move_byte   () const;
    U8          get_action_byte () const;
    U8          get_pos_byte    () const;
    char const* get_data_block  () const;
    int         get_size        () const;

    bool        header_contains (U8 header) const;
    bool        move_contains   (U8 input)  const;
    bool        action_contains (U8 input)  const;
    bool        position_is     (U8 pos)    const;
    char*       to_char_array   ()          const;

    void        parse_buffer    (char const* buffer);
    void        reset           ();

    void        set_header      (U8 const header);
    void        set_input_move  (U8 const input);
    void        set_input_action(U8 const input);
    void        set_pos         (U8 const pos);
    void        set_data        (char const* data);
    void        set_data_byte   (int const byte, U8 const data);

    void        print() const; // Debug function
};

} // namespace network

#endif // NETWORK_UTILITIES_UDP_PACKET
