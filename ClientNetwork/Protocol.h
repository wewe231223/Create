#pragma once

#pragma pack(push, 1)
struct Packet {
    unsigned char size;
    unsigned char type;
    unsigned char id;
};

struct PacketChatting {
    char chatBuffer[100];
};
#pragma pack(pop)