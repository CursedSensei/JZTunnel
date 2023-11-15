#include "Frameworks.h"

int main() {

	if (initializeWinMutex()) {
		return 1;
	}

	p_Sockets_Data SocketData = initializeSocketDataVar();
	if (SocketData == nullptr) {
		return 1;
	}

	printf("Connecting . . .\r");
	Connect_to_Server(SocketData);
	printf("Connected!           \n");
	waitforSockets(SocketData);
	freeSocketDataVar(SocketData);

	return 0;
}