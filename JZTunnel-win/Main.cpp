#include "Frameworks.h"

int main() {

	if (initializeWinMutex()) {
		return 1;
	}

	p_Sockets_Data SocketData = initializeSocketDataVar();
	if (SocketData == nullptr) {
		return 2;
	}

	printf("Connecting . . .\n");
	Connect_to_Server(SocketData);
	waitforSockets(SocketData);
	freeSocketDataVar(SocketData);

	return 0;
}