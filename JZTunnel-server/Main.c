#include "Frameworks.h"

int main() {
	SOCKET listenerSocket = getListenerSocket();
	p_Listener_Pipe listenPipe = getListenerPipe();

	while (1) {
		SOCKET clientSocket = getClientSocket(listenerSocket);
		if (checkSocket(clientSocket)) continue;
		else clientStatus = TRUE;

		// to deployments

		cleanClientSocket(clientSocket, listenPipe);
	}

	close(listenerSocket);

	return 0;
}