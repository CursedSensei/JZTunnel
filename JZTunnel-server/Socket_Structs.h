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

typedef struct {
    unsigned char extra[8];
    unsigned short int protocol;
    unsigned short int checksum;
    struct in_addr src_addr;
    struct in_addr dest_addr;
} IP_Header, *p_IP_Header;

typedef struct {
    uint16_t src_port;
    uint16_t dest_port;
    uint16_t ext;
    uint16_t checksum;
} UDP_Header, *p_UDP_Header;

typedef struct {
    uint16_t src_port;
    uint16_t dest_port;
    unsigned int ext[3];
    uint16_t checksum;
    uint16_t urg_ptr;
} TCP_Header, *p_TCP_Header;