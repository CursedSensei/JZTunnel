#include "Frameworks.h"

int main() {
	SOCKET listenerSocket = getListenerSocket();

	while (1) {
		SOCKET clientSocket = getClientSocket(listenerSocket);
		if (checkSocket(clientSocket)) continue;

		// to deployments
	}

	close(listenerSocket);

	return 0;
}