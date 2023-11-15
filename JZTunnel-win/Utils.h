#pragma once

int initializeWinMutex() {
	LPCSTR winMutexText = "JZTunnel Application";

	HWND winMutexWindow = FindWindowA(winMutexText, "Client");

	if (winMutexWindow) {
		return 1;
	}

	WNDCLASSA winMutex;
	ZeroMemory(&winMutex, sizeof(WNDCLASSA));

	winMutex.hInstance = (HINSTANCE)GetModuleHandleA(NULL);
	winMutex.lpszClassName = winMutexText;
	winMutex.lpfnWndProc = DefWindowProc;

	RegisterClassA(&winMutex);
	//Create window

	winMutexWindow = CreateWindowA(
		winMutexText,
		"Client",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		(HINSTANCE)GetModuleHandleA(NULL),
		NULL);

	if (!winMutexWindow) return 1;

	//CloseHandle(winMutexWindow); unknown reason but invalid handle

	return 0;
} 

p_Sockets_Data initializeSocketDataVar() {
	unsigned int clientport = 0;

	while (!clientport || clientport > 65535) {
		printf("Input Port to open (1 - 65535): ");
		scanf_s("%u", &clientport);
	}

	WSADATA WSAdata;
	if (WSAStartup(MAKEWORD(2, 2), &WSAdata)) {
		return nullptr;
	}

	p_Sockets_Data SocketStatus = (p_Sockets_Data)malloc(sizeof(Sockets_Data));
	SocketStatus->client_port = (unsigned short int)clientport;
	SocketStatus->clientports = (unsigned short int*)malloc(0);
	SocketStatus->clientsockets = (SOCKET*)malloc(0);

	return SocketStatus;
}

void waitforSockets(p_Sockets_Data SocketStatus) {
	while (SocketStatus->status != 3) {
		Sleep(30);
	}
}

void freeSocketDataVar(p_Sockets_Data SocketStatus) {
	WSACleanup();

	free(SocketStatus->clientports);
	free(SocketStatus);
}

void sendHandshake(SOCKET ServerSocket) {
	Handshake_Packet handshake;
	send(ServerSocket, (char *)&handshake, sizeof(Handshake_Packet), 0);
}