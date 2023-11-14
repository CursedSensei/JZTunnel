#pragma once

SOCKET getListenerSocket() {
    SOCKET clientsock;

    while (1) {
        clientsock = socket(AF_INET, SOCK_STREAM, 0);
        if (checkSocket(clientsock)) {
            continue;;
        }

        struct sockaddr_in dest_addr;
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(30000);
        dest_addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(clientsock, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) >= 0) {
            break;
        }
        close(clientsock);
    }

    return clientsock;
}

SOCKET getClientSocket(SOCKET listenersock) {
    if (listen(listenersock, 5) < 0) {
        return SOCKET_ERROR;
    }

    SOCKET clientsock = accept(listenersock, NULL, NULL);
    if (checkSocket(clientsock)) {
        return SOCKET_ERROR;
    }

    p_Handshake_Packet recvhandshake;

    if (recv(clientsock, (void *)recvhandshake, sizeof(Handshake_Packet), 0) <= 0) {
        shutdown(clientsock, SHUT_RDWR);
        close(clientsock);
        return SOCKET_ERROR;
    }

    if (verifyHandshake(recvhandshake)) {
        return clientsock;
    } else {
        shutdown(clientsock, SHUT_RDWR);
        close(clientsock);
        return SOCKET_ERROR;
    }
}

PTHREAD_FUNCTION listenerThread(void *args) {
    p_Listener_Pipe listenPipe = (p_Listener_Pipe) args;
    
    while (1) {
        while (!clientStatus) {
            sleep(5);
        }

        while (clientStatus) {
            Tunnel_Packet packetRecv;
            struct sockaddr_in recvAddr;

            if (recvfrom(listenPipe->listenerSocket, (void *)packetRecv.data, PACKET_SIZE - 2, 0, (struct sockaddr *)&recvAddr, sizeof(struct sockaddr_in)) > 0) {
                packetRecv.id = getAddrId(&recvAddr, listenPipe);

                send(listenPipe->clientSocket, (void *)&packetRecv, PACKET_SIZE, 0);
            }
        }
    }
}

void deployListenerThread(p_Listener_Pipe listenerPipe) {
    pthread_t pid;

    pthread_create(&pid, NULL, listenerThread, (void *)listenerPipe);
}