#pragma once

typedef struct {
    char name[7];
    unsigned int pass;
} Handshake_Packet, *p_Handshake_Packet;

typedef struct {
    char ip[17];
    unsigned short int port;
}address, *p_address;

typedef struct {
    p_address addresses;
    unsigned short int addrLen;
    SOCKET clientSocket;
    SOCKET listenerSocket;
} Listener_Pipe, *p_Listener_Pipe;

typedef struct {
	unsigned short int id;
	char data[PACKET_SIZE - 2];
} Tunnel_Packet, p_Tunnel_Packet;