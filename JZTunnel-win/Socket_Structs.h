#pragma once

typedef struct tagSockets_Data {
	short int client_port = 0;
	short int status = 0;
	SOCKET ServerSocket = INVALID_SOCKET;
	unsigned short int* clientports = (unsigned short int *)malloc(0);
} Sockets_Data, *p_Sockets_Data;

typedef struct tagClientSocketParam {
	short int id;
	p_Sockets_Data SocketStatus;
}ClientSocketParam, *p_ClientSocketParam;