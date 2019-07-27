#include "udp_packet.hpp"

#include <cstdio>

namespace network {

UdpPacket::UdpPacket()
{
    reset();
}

UdpPacket::UdpPacket(char buffer[])
    : UdpPacket()
{
    parse_buffer(buffer);
}

UdpPacket::UdpPacket(U8 const header)
    : UdpPacket()
{
    set_header(header);
}

UdpPacket::UdpPacket(U8 const header, char data[])
    : UdpPacket(header)
{
    set_data(data);
}

UdpPacket::UdpPacket(U8 const header, U8 const pos)
    : UdpPacket(header)
{
    set_pos(pos);
}

UdpPacket::UdpPacket(U8 const header, U8 const pos, char data[])
    : UdpPacket(header, pos)
{
    set_data(data);
}


UdpPacket::UdpPacket(U8 const header, U8 const input_move, U8 const input_action, char data[])
    : UdpPacket(header, data)
{
    set_input_move(input_move);
    set_input_action(input_action);
}

UdpPacket::UdpPacket(U8 const header, U8 const input_move, U8 const input_action, U8 const pos)
    : UdpPacket(header, pos)
{
    set_input_move(input_move);
    set_input_action(input_action);
}

UdpPacket::UdpPacket(U8 const header, U8 const input_move, U8 const input_action, U8 const pos, char data[])
    : UdpPacket(header, input_move, input_action, data)
{
    set_pos(pos);
}

UdpPacket::~UdpPacket()
{ }

U8 UdpPacket::get_header_byte() const
{
    return m_packet[m_header_byte_i];
}

U8 UdpPacket::get_move_byte() const
{
    return m_packet[m_move_byte_i];
}

U8 UdpPacket::get_action_byte() const
{
    return m_packet[m_action_byte_i];
}

U8 UdpPacket::get_pos_byte() const
{
    return m_packet[m_pos_byte_i];
}

char const* UdpPacket::get_data_block() const
{
    return &m_packet[m_datablock_i];
}

bool UdpPacket::header_contains(U8 header) const
{
    return (get_header_byte() & header) != 0;
}

bool UdpPacket::move_contains(U8 input) const
{
    return (get_move_byte() & input) != 0;
}

bool UdpPacket::action_contains(U8 input) const
{
    return (get_action_byte() & input) != 0;
}

bool UdpPacket::position_is(U8 pos) const
{
    return get_pos_byte() == pos;
}

int UdpPacket::get_size() const
{
    auto size = 0;
    while (get_data_block()[size] != '\0')
        size++;
    if (get_header_byte() != UDP_H_NULL)
        size++;

    // add input byte size if there is content
    // (may be empty in game logic standards, but still has size of 1)
    if (get_data_block()[0] != '\0')
    {
        size++; // move byte
        size++; // action byte
        size++; // position byte
    }
    else
    {
        if (get_pos_byte() != UDP_H_NULL)
            size += 3;
        else if (get_action_byte() != UDP_H_NULL)
            size += 2;
        else if (get_move_byte() != UDP_H_NULL)
            size++;
    }

    return size;
}

void UdpPacket::parse_buffer(char const* buffer)
{
    set_header(buffer[m_header_byte_i]);

    if (header_contains(UDP_H_INPUT))
    {
        set_input_move(buffer[m_move_byte_i]);
        set_input_action(buffer[m_action_byte_i]);
    }

    if (header_contains(UDP_H_POS))
        set_pos(buffer[m_pos_byte_i]);

    if (!header_contains(UDP_H_DATABLOCK))
        return;

    auto i = 0;
    auto byte = m_datablock_i;
    while (buffer[byte] != '\0')
        set_data_byte(i++, buffer[byte++]);
}

void UdpPacket::reset()
{
    for (auto i = 0; i < UDP_MAX_PACKET_SIZE; i++)
        m_packet[i] = UDP_H_NULL;
}

void UdpPacket::set_header(U8 const header)
{
    m_packet[m_header_byte_i] = header;
}

void UdpPacket::set_input_move(U8 const input)
{
    m_packet[m_move_byte_i] = input;
}

void UdpPacket::set_input_action(U8 const input)
{
    m_packet[m_action_byte_i] = input;
}

void UdpPacket::set_pos(U8 const pos)
{
    m_packet[m_pos_byte_i] = pos;
}

void UdpPacket::set_data(char const* data)
{
    auto i = 0;
    while (data[i] != '\0')
    {
        set_data_byte(i, data[i]);
        i++;
    }
}

void UdpPacket::set_data_byte(int const byte, U8 const data)
{
    m_packet[m_datablock_i + byte] = data;
}

char* UdpPacket::to_char_array() const
{
    return const_cast<char*>(m_packet);
}

void UdpPacket::print() const
{
    // Debug function
    if (header_contains(UDP_H_NULL))
        printf("           > Header     : UDP_H_NULL\n");
    if (header_contains(UDP_H_KEEPALIVE))
        printf("           > Header     : UDP_H_KEEPALIVE\n");
    if (header_contains(UDP_H_OK))
        printf("           > Header     : UDP_H_OK\n");
    if (header_contains(UDP_H_ERROR))
        printf("           > Header     : UDP_H_ERROR\n");
    if (header_contains(UDP_H_CONNECT))
        printf("           > Header     : UDP_H_CONNECT\n");
    if (header_contains(UDP_H_INPUT))
        printf("           > Header     : UDP_H_INPUT\n");
    if (header_contains(UDP_H_POS))
        printf("           > Header     : UDP_H_POS\n");
    //if (header_contains(UDP_H_ADDITIONAL))
    //    printf("           > Header     : UDP_H_ADDITIONAL\n");
    if (header_contains(UDP_H_DATABLOCK))
        printf("           > Header     : UDP_H_DATABLOCK\n");

    if (header_contains(UDP_H_INPUT) &&
        (move_contains(UDP_IN_PRESS) || 
         action_contains(UDP_IN_PRESS)))
    {
        if (move_contains(UDP_IN_UP))
            printf("           > Input      : UP press\n");
        if (move_contains(UDP_IN_DOWN))
            printf("           > Input      : DOWN press\n");
        if (move_contains(UDP_IN_LEFT))
            printf("           > Input      : LEFT press\n");
        if (move_contains(UDP_IN_RIGHT))
            printf("           > Input      : RIGHT press\n");
        if (action_contains(UDP_IN_SHOOT))
            printf("           > Input      : SHOOT press\n");
        if (action_contains(UDP_IN_RELOAD))
            printf("           > Input      : RELOAD press\n");
        if (action_contains(UDP_IN_SWITCH_WPN_1))
            printf("           > Input      : SW_WPN_1 press\n");
        if (action_contains(UDP_IN_SWITCH_WPN_2))
            printf("           > Input      : SW_WPN_2 press\n");
        if (action_contains(UDP_IN_SWITCH_WPN_3))
            printf("           > Input      : SW_WPN_3 press\n");
    }
    else if (header_contains(UDP_H_INPUT))
    {
        if (move_contains(UDP_IN_UP))
            printf("           > Input      : UP release\n");
        if (move_contains(UDP_IN_DOWN))
            printf("           > Input      : DOWN release\n");
        if (move_contains(UDP_IN_LEFT))
            printf("           > Input      : LEFT release\n");
        if (move_contains(UDP_IN_RIGHT))
            printf("           > Input      : RIGHT release\n");
        if (action_contains(UDP_IN_SHOOT))
            printf("           > Input      : SHOOT release\n");
    }

    if (header_contains(UDP_H_POS))
        printf("           > Position   : %c\n", get_pos_byte());

    if (header_contains(UDP_H_DATABLOCK))
        printf("           > Data       : %s\n", get_data_block());
}

} // namespace network
