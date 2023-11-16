#pragma once

short int GetClientSocket(p_ClientSocketParam clientparams) {
	clientparams->SocketStatus->clientsockets[clientparams->id - 1] = socket(AF_INET, SOCK_RAW, 0);
	if (clientparams->SocketStatus->clientsockets[clientparams->id - 1] == INVALID_SOCKET) {
		return CLIENT_ERROR;
	}

	sockaddr_in sockAddr;

	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	sockAddr.sin_port = 0;

	if (bind(clientparams->SocketStatus->clientsockets[clientparams->id - 1], (sockaddr*)&sockAddr, sizeof(sockAddr))) {
		closesocket(clientparams->SocketStatus->clientsockets[clientparams->id - 1]);
		return CLIENT_ERROR;
	}

	clientparams->SocketStatus->clientports[clientparams->id - 1] = ntohs(sockAddr.sin_port);

	return 0;
}

DWORD WINAPI ClientSocket(LPVOID args) {
	p_ClientSocketParam clientparams = (p_ClientSocketParam)args;

	if (GetClientSocket(clientparams) == CLIENT_ERROR) {
		printf("Client listener Error!\n");

		clientparams->SocketStatus->clientsockets[clientparams->id - 1] = INVALID_SOCKET;
		clientparams->SocketStatus->clientports[clientparams->id - 1] = CLIENT_ERROR;
		free(clientparams);
		return 0;
	}

	int clientReceived;
	sockaddr_in outAddr;
	int outAddrlen = sizeof(outAddr);

	printf("Client Listener deployed!\n");

	do {
		Tunnel_Packet sendpack;

		clientReceived = recvfrom(clientparams->SocketStatus->clientsockets[clientparams->id - 1], (char *)&sendpack.data, PACKET_SIZE - 2, 0, (sockaddr*)&outAddr, &outAddrlen);
		if (clientReceived <= 0) {
			continue;
		}

		sendpack.id = clientparams->id;

		send(clientparams->SocketStatus->ServerSocket, (const char*)&sendpack, PACKET_SIZE, 0);

	} while (clientReceived > 0);

	closesocket(clientparams->SocketStatus->clientsockets[clientparams->id - 1]);
	clientparams->SocketStatus->clientsockets[clientparams->id - 1] = INVALID_SOCKET;
	clientparams->SocketStatus->clientports[clientparams->id - 1] = CLIENT_ERROR;
	free(clientparams);

	return 0;
}

void DeployClientListener(const unsigned short int id, p_Sockets_Data SocketStatus) {
	p_ClientSocketParam clientargs = (p_ClientSocketParam)malloc(sizeof(ClientSocketParam));

	clientargs->id = id;
	clientargs->SocketStatus = SocketStatus;

	if (SocketStatus->lenports < id - 1) {
		SocketStatus->lenports++;
		SocketStatus->clientports = (unsigned short int*)realloc(SocketStatus->clientports, SocketStatus->lenports * sizeof(unsigned short int));
		SocketStatus->clientports[id - 1] = 0;
		SocketStatus->clientsockets = (SOCKET*)realloc(SocketStatus->clientsockets, SocketStatus->lenports * sizeof(SOCKET));
		SocketStatus->clientsockets[id - 1] = INVALID_SOCKET;
	}

	for (int i = 0; i < 5; i++) {
		HANDLE clientHandle = CreateThread(NULL, 0, ClientSocket, (LPVOID)clientargs, 0, NULL);

		if (clientHandle) {
			CloseHandle(clientHandle);
			return;
		}
	}

	free(clientargs);
	SocketStatus->clientsockets[id - 1] = INVALID_SOCKET;
	SocketStatus->clientports[id - 1] = CLIENT_ERROR;
}

SOCKET GetServerSocket() {
	addrinfo sockaddr;
	PADDRINFOA hostinfo = NULL;

	ZeroMemory(&sockaddr, sizeof(sockaddr));
	sockaddr.ai_family = AF_INET;
	sockaddr.ai_socktype = SOCK_STREAM;
	sockaddr.ai_protocol = IPPROTO_TCP;

	SOCKET MainSock = INVALID_SOCKET;

	if (getaddrinfo(TUNNEL_IP, TUNNEL_PORT, &sockaddr, &hostinfo)) {
		return INVALID_SOCKET;
	}

	for (hostinfo; hostinfo != NULL; hostinfo = hostinfo->ai_next) {

		MainSock = socket(hostinfo->ai_family, hostinfo->ai_socktype, hostinfo->ai_protocol);
		if (MainSock == INVALID_SOCKET) {
			freeaddrinfo(hostinfo);
			return INVALID_SOCKET;
		}

		if (connect(MainSock, hostinfo->ai_addr, (int)hostinfo->ai_addrlen) == SOCKET_ERROR) {
			closesocket(MainSock);
			MainSock = INVALID_SOCKET;
			continue;
		}
		break;
	}

	if (MainSock == INVALID_SOCKET) return INVALID_SOCKET;

	else freeaddrinfo(hostinfo);

	return MainSock;
}

DWORD WINAPI ServerSocket(LPVOID args) {
	p_Sockets_Data SocketStatus = (p_Sockets_Data)args;

	SocketStatus->ServerSocket = GetServerSocket();
	if (SocketStatus->ServerSocket == INVALID_SOCKET) {
		SocketStatus->status = 3;
		return 0;
	}

	printf("Sending Handshake!\n");

	sendHandshake(SocketStatus->ServerSocket);

	printf("Handshake Sent!\n");

	SocketStatus->status = 1;
	printf("Connected!\n");

	int sockReceived;
	sockaddr_in destAddr;
	destAddr.sin_family = AF_INET;
	destAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	do {
		Tunnel_Packet recvpacket;
		sockReceived = recv(SocketStatus->ServerSocket, (char *)&recvpacket, PACKET_SIZE, 0);

		if (sockReceived <= 0) {
			continue;
		}

		if (recvpacket.id > SocketStatus->lenports ||
			SocketStatus->clientports[recvpacket.id - 1] == CLIENT_ERROR ||
			(SocketStatus->clientsockets[recvpacket.id - 1] == INVALID_SOCKET && SocketStatus->clientports[recvpacket.id - 1] != 0))
		{

			DeployClientListener(recvpacket.id, SocketStatus);

			while (SocketStatus->clientports[recvpacket.id - 1] == 0) {
				Sleep(1);
			}

			if (SocketStatus->clientports[recvpacket.id - 1] == INVALID_SOCKET) {
				continue;
			}
		}

		destAddr.sin_port = htons(SocketStatus->clientports[recvpacket.id - 1]);

		sendto(SocketStatus->clientsockets[recvpacket.id - 1], (const char *)&recvpacket.data, sizeof(recvpacket.data), 0, (struct sockaddr*)&destAddr, sizeof(destAddr));
	} while (sockReceived >= 0);

	SocketStatus->status = 3;
	closesocket(SocketStatus->ServerSocket);

	return 0;
}

void Connect_to_Server(p_Sockets_Data SocketStatus) {

	HANDLE ServerThread = CreateThread(NULL, 0, ServerSocket, (LPVOID)SocketStatus, 0, NULL);
	if (!ServerThread) {
		SocketStatus->status = 3;
		return;
	}

	CloseHandle(ServerThread);

	while (SocketStatus->status == 0) {
		Sleep(1);
	}
}

