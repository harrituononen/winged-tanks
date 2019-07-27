#ifndef NETWORK_CONFIG_HPP
#define NETWORK_CONFIG_HPP

namespace network {

typedef unsigned char U8;

char const              SERVER_DEFAULT_NAME[]   = "Test server #1";
char const              SERVER_DEFAULT_ADDR[]   = "127.0.0.1";
int const               SERVER_DEFAULT_PORT     = 51225; // [49152, 65535]

int const               ROOM_MAX_USERS          = 4;
int const               ROOM_MAX_NUM            = 64;

int const               NETWORK_ID_LENGTH       = 40;

int const               THROUGHPUT_LAG_MS       = 50;
int const               PING_FREQUENCY_MS       = 2000;
int const               SYNC_FREQUENCY_MS       = 51;
int const               CURSOR_SYNC_FREQUENCY_MS= 41;
int const               MISSED_PINGS_ALLOWED    = 4;

int const               UDP_TOTAL_SIZE          = 512;
int const               UDP_MAX_PACKET_SIZE     = UDP_TOTAL_SIZE - 20;
int const               UDP_MAX_DATABLOCK_SIZE  = UDP_MAX_PACKET_SIZE
                                                - NETWORK_ID_LENGTH
                                                - 3;

// UDP packet composition so far (not final):
//  header    input     pos1      pos2      datablock
// [--------][--------][--------][--------][-...   ...-]
//  1 byte    1 byte    1 byte    1 byte     x bytes

// ------------------------------------------------------ HEADER BYTE
unsigned char const     UDP_H_NULL              = 0;
unsigned char const     UDP_H_OK                = 1;
unsigned char const     UDP_H_ERROR             = 1 << 1;
unsigned char const     UDP_H_CONNECT           = 1 << 2;
unsigned char const     UDP_H_KEEPALIVE         = 1 << 3;
unsigned char const     UDP_H_INPUT             = 1 << 4;
unsigned char const     UDP_H_POS               = 1 << 5;
unsigned char const     UDP_H_DATABLOCK         = 1 << 6;
//unsigned char const     UDP_H_ADDITIONAL        = 1 << 7;

// ------------------------------------------------------ MOVE INPUT BYTE
unsigned char const     UDP_IN_PRESS            = 1;
unsigned char const     UDP_IN_UP               = 1 << 1;
unsigned char const     UDP_IN_DOWN             = 1 << 2;
unsigned char const     UDP_IN_LEFT             = 1 << 3;
unsigned char const     UDP_IN_RIGHT            = 1 << 4;
// ------------------------------------------------------ ACTION INPUT BYTE
unsigned char const     UDP_IN_SHOOT            = 1 << 1;
unsigned char const     UDP_IN_RELOAD           = 1 << 2;
unsigned char const     UDP_IN_SWITCH_WPN_1     = 1 << 3;
unsigned char const     UDP_IN_SWITCH_WPN_2     = 1 << 4;
unsigned char const     UDP_IN_SWITCH_WPN_3     = 1 << 5;
unsigned char const     UDP_IN_SWITCH_WPN_Q     = 1 << 6;

// ------------------------------------------------------ POS_SYNC BYTE
unsigned char const     UDP_POS_PLAYER_1         = 1;
unsigned char const     UDP_POS_PLAYER_2         = 1 << 1;
unsigned char const     UDP_POS_PLAYER_3         = 1 << 2;
unsigned char const     UDP_POS_PLAYER_4         = 1 << 3;
unsigned char const     UDP_POS_TEAM_1           = 1 << 4;
unsigned char const     UDP_POS_TEAM_2           = 1 << 5;
unsigned char const     UDP_POS_EXPLOSION        = 1 << 6;

} // namespace network

#endif // NETWORK_CONFIG_HPP
