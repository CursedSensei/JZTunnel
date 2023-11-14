#include "Frameworks.h"

int main() {
	SOCKET listenerSocket = getListenerSocket();
	p_Listener_Pipe listenPipe = getListenerPipe(listenerSocket);
	deployListenerThread(listenPipe);

	while (1) {
		SOCKET clientSocket = getClientSocket(listenerSocket);
		if (checkSocket(clientSocket)) continue;
		else clientStatus = TRUE;

		// listen to client

		cleanClientSocket(clientSocket, listenPipe);
	}

	close(listenerSocket);

	return 0;
}