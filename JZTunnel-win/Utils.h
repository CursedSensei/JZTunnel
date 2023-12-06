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
	WSADATA WSAdata;
	if (WSAStartup(MAKEWORD(2, 2), &WSAdata)) {
		return nullptr;
	}

	SOCKET testSocket = socket(AF_INET, SOCK_RAW, 0);
	if (testSocket == INVALID_SOCKET) {
		if (WSAGetLastError() == WSAEACCES) {
			printf("Program must have admin privileges.\n");
			ShellExecuteA(NULL, "runas", "JZTunnel-win.exe", NULL, NULL, SW_SHOW);
		}
		WSACleanup();
		return nullptr;
	}
	closesocket(testSocket);
	
	unsigned int clientport = 0;

	while (!clientport || clientport > 65535) {
#if __DEBUG__
		printf("Input Port to open (1 - 65535): ");
		scanf_s("%u", &clientport);
#else
		printf("Implement me!\n\n");
		printf("Input Port to open (1 - 65535): ");
		scanf_s("%u", &clientport);
#endif
	}

	p_Sockets_Data SocketStatus = (p_Sockets_Data)malloc(sizeof(Sockets_Data));
	SocketStatus->client_port = htons((u_short)clientport);
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

unsigned short int checksum(unsigned short int* buf, size_t buflen, unsigned short int* pseudo_hdr) {
	unsigned long long int sum = 0;
	if (pseudo_hdr != nullptr) {
		int pseudolen = sizeof(Pseudo_Header);
		while (pseudolen) {
			sum += *pseudo_hdr++;
			pseudolen -= 2;
		}
	}

	while (buflen > 1) {
		sum += *buf++;
		buflen -= 2;
	}

	if (buflen) {
		sum += *(unsigned char *)buf;
	}

	while (sum >> 16) {
		sum = (sum >> 16) + (sum & 0xffff);
	}

	return ~sum;
}
