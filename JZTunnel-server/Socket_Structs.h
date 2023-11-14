#pragma once

typedef struct {
    char name[7];
    unsigned int pass;
} Handshake_Packet, *p_Handshake_Packet;

typedef struct {
    char ip[17];
    short int port;
}address, *p_address;

typedef struct {
    p_address addresses;
    SOCKET clientSocket;
} Listener_Pipe, *p_Listener_Pipe;