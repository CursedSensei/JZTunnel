#pragma once

typedef struct tagSockets_Data {
	unsigned short int client_port = 0;
	char status = 0;
	SOCKET ServerSocket = INVALID_SOCKET;
	unsigned short int* clientports = (unsigned short int*)malloc(0);
	SOCKET* clientsockets = (SOCKET*)malloc(0);
	size_t lenports = 0;
} Sockets_Data, *p_Sockets_Data;

typedef struct tagClientSocketParam {
	unsigned short int id;
	p_Sockets_Data SocketStatus;
}ClientSocketParam, *p_ClientSocketParam;

struct Tunnel_Packet {
	unsigned short int id;
	BYTE data[PACKET_SIZE - 2];
};

struct Handshake_Packet {
	char name[7] = "CLIENT";
	unsigned int pass = 0xDE4A5FBA;
};

typedef struct {
    unsigned char extra[8];
    unsigned char ttl;
    unsigned char protocol;
    unsigned short checksum;
    struct in_addr src_addr;
    struct in_addr dest_addr;
} IP_Header, * p_IP_Header;

typedef struct {
    unsigned short src_port;
    unsigned short dest_port;
    unsigned short ext;
    unsigned short checksum;
} UDP_Header, * p_UDP_Header;

typedef struct {
    unsigned short src_port;
    unsigned short dest_port;
    unsigned int ext[3];
    unsigned short checksum;
    unsigned short urg_ptr;
} TCP_Header, * p_TCP_Header;

typedef struct {
    unsigned int src_addr;
    unsigned int dest_addr;
    unsigned char reserved;
    unsigned char protocol;
    unsigned short length;
} Pseudo_Header, * p_Pseudo_Header;