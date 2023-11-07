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
	short int id;
	BYTE data[PACKET_SIZE - 2];
};