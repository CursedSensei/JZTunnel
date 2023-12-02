#pragma once

typedef struct {
    char name[7];
    unsigned int pass;
} Handshake_Packet, *p_Handshake_Packet;

typedef struct {
    struct in_addr ip;
    uint16_t port;
    uint8_t mac[6];
} address, *p_address;

typedef struct {
    p_address addresses;
    unsigned short int addrLen;
    struct in_addr listenerAddr;
    uint8_t listenerMac[6];
    SOCKET clientSocket;
    SOCKET listenerSocket;
    SOCKET listenerBinder;
} Listener_Pipe, *p_Listener_Pipe;

typedef struct {
	unsigned short int id;
	char data[PACKET_SIZED - 2];
} Tunnel_Packet, p_Tunnel_Packet;

typedef struct {
    uint8_t extra[8];
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
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

typedef struct {
    uint32_t src_addr;
    uint32_t dest_addr;
    uint8_t reserved;
    uint8_t protocol;
    uint16_t length;
} Pseudo_Header, *p_Pseudo_Header;