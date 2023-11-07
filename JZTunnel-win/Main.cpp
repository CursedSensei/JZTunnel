#include "Frameworks.h"

int main() {

	if (initializeWinMutex()) {
		return 1;
	}

	p_Sockets_Data SocketData = initializeSocketDataVar();
	if (SocketData == nullptr) {
		return 1;
	}

	Connect_to_Server(SocketData);



	/*
	https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-socket
	https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-bind
	https://learn.microsoft.com/en-us/windows/win32/winsock/sockaddr-2
	https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-sendto
	https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-recvfrom
	*/

	return 0;
}