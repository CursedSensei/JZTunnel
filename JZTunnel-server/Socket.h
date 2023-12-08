#pragma once

void getListenerSocket(p_Listener_Pipe listenPipe) {
    SOCKET clientsock;
    SOCKET listenersock;

    while (TRUE) {
        clientsock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP));
        if (!checkSocket(clientsock)) {
            break;
        }
        printf("Socket Error\n");
    }

    while (TRUE) {
        listenersock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
        if (checkSocket(listenersock)) {
            continue;
        }

        struct sockaddr_in dest_addr;
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = TUNNEL_PORT;
        dest_addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(listenersock, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) >= 0) {
            shutdown(listenersock, SHUT_RDWR);
            break;
        }
        close(listenersock);
    }

    printf("Listener created\n");

    listenPipe->listenerSocket = clientsock;
    listenPipe->listenerBinder = listenersock;
}

SOCKET getClientSocket() {
    SOCKET listenersock;

    while (TRUE) {
        listenersock = socket(AF_INET, SOCK_STREAM, 0);
        if (checkSocket(listenersock)) {
            continue;
        }

        struct sockaddr_in dest_addr;
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(CLIENT_PORT);
        dest_addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(listenersock, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) >= 0) {
            break;
        }
        close(listenersock);
    }


    if (listen(listenersock, 5) < 0) {
        close(listenersock);
        return SOCKET_ERROR;
    }

    SOCKET clientsock = accept(listenersock, NULL, NULL);
    if (checkSocket(clientsock)) {
        close(listenersock);
        return SOCKET_ERROR;
    }

    Handshake_Packet recvhandshake;

    if (recv(clientsock, (void *)&recvhandshake, sizeof(Handshake_Packet), 0) <= 0) {
        shutdown(clientsock, SHUT_RDWR);
        close(clientsock);
        close(listenersock);
        return SOCKET_ERROR;
    }

    if (verifyHandshake(&recvhandshake)) {
        close(listenersock);
        return clientsock;
    } else {
        shutdown(clientsock, SHUT_RDWR);
        close(clientsock);
        close(listenersock);
        
        return SOCKET_ERROR;
    }
}

PTHREAD_FUNCTION listenerThread(void *args) {
    p_Listener_Pipe listenPipe = (p_Listener_Pipe) args;
    
    while (TRUE) {
        printf("Listener waiting for client\n");

        while (!clientStatus) {
            sleep(5);
        }

        printf("to listen\n");

        int bytesReceived = 0;
        Tunnel_Packet packetRecv;
        Link_Layer_Packet linkRecv;

        memset(&packetRecv, 0, PACKET_SIZE);

        while (clientStatus) {
            bytesReceived = recv(listenPipe->listenerSocket, (void *)&linkRecv, PACKET_SIZE + sizeof(struct ether_header) - 2, 0);

            if (bytesReceived > 0 && clientStatus) {

                if (checkPacket((unsigned char *)&linkRecv, listenPipe)) {
                    packetRecv.id = getAddrId((unsigned char *)&linkRecv, listenPipe);
                    memcpy(packetRecv.data, linkRecv.data, bytesReceived - sizeof(struct ether_header));
                    
                    send(listenPipe->clientSocket, (void *)&packetRecv, bytesReceived - sizeof(struct ether_header) + 2, 0);
                    memset(&packetRecv, 0, bytesReceived - sizeof(struct ether_header) + 2);
                }

                memset(&linkRecv, 0, bytesReceived);
            } else printf("pass or error packet \n");
        }
    }
}

void deployListenerThread(p_Listener_Pipe listenerPipe) {
    pthread_t pid;

    pthread_create(&pid, NULL, listenerThread, (void *)listenerPipe);
}
