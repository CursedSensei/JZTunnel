#pragma once

void DeployClientListener(const short int id, p_Sockets_Data SocketStatus) {
	p_ClientSocketParam clientargs = (p_ClientSocketParam)malloc(sizeof(ClientSocketParam));

	clientargs->id = id;
	clientargs->SocketStatus = SocketStatus;

	SocketStatus->clientports = (unsigned short int*)realloc(SocketStatus->clientports, sizeof(SocketStatus->clientports) + sizeof(unsigned short int));
	SocketStatus->clientports[id - 1] = 0;

	for (int i = 0; i < 5; i++) {
		HANDLE clientHandle = CreateThread(NULL, 0, ClientSocket, (LPVOID)clientargs, 0, NULL);

		if (clientHandle) {
			CloseHandle(clientHandle);
			return;
		}
	}

	free(clientargs);
	SocketStatus->clientports[id - 1] = CLIENT_ERROR;
}

DWORD WINAPI ClientSocket(LPVOID args) {
	p_ClientSocketParam clientparams = (p_ClientSocketParam)args;


	free(clientparams);
}

DWORD WINAPI ServerSocket(LPVOID args) {
	p_Sockets_Data SocketStatus = (p_Sockets_Data)args;

}

void Connect_to_Server(p_Sockets_Data SocketStatus) {

	

	while (SocketStatus->status == 0) {
		Sleep(1);
	}
}

