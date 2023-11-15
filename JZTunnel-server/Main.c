#include "Frameworks.h"

int main() {
	SOCKET listenerSocket = getListenerSocket();
	p_Listener_Pipe listenPipe = getListenerPipe(listenerSocket);
	deployListenerThread(listenPipe);

	while (TRUE) {
		printf("Waiting for client\n");

		SOCKET clientSocket = getClientSocket(listenerSocket);
		if (checkSocket(clientSocket)) continue;
		else clientStatus = TRUE;

		printf("Connected to client\n");

		Tunnel_Packet clientPacket;

		for (int recvStatus = 0; recvStatus >= 0; recvStatus = recv(clientSocket, (void *)&clientPacket, PACKET_SIZE, 0)) {

			if (recvStatus > 0) {
				struct sockaddr_in destAddr;
				destAddr.sin_family = AF_INET;
				destAddr.sin_addr.s_addr = inet_addr(listenPipe->addresses[clientPacket.id - 1].ip);
				destAddr.sin_port = htons(listenPipe->addresses[clientPacket.id - 1].port);

				sendto(listenerSocket, (void *)clientPacket.data, PACKET_SIZE - 2, 0, (struct sockaddr *)&destAddr, sizeof(struct sockaddr_in));
			}
		}

		cleanClientSocket(clientSocket, listenPipe);
	}

	close(listenerSocket);

	return 0;
}