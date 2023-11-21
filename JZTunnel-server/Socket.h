#pragma once

SOCKET getListenerSocket() {
    SOCKET clientsock;

    while (TRUE) {
        clientsock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
        if (checkSocket(clientsock)) {
            printf("Socket Error\n");
            continue;
        }

        unsigned char one = TRUE;

        if (setsockopt(clientsock, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
            close(clientsock);
            printf("setsockopt 1 Error\n");
            continue;
        }

        // if (setsockopt(clientsock, IPPROTO_IP, IPPROTO_RAW, &one, sizeof(one)) < 0) {
        //     printf("setsockopt 2 Error\n");
        //     close(clientsock);
        //     continue;
        // }

        printf("To bind\n");

        struct sockaddr_in dest_addr;
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(TUNNEL_PORT);
        dest_addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(clientsock, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) >= 0) {
            break;
        }
        close(clientsock);
    }

    return clientsock;
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

        while (clientStatus) {
            Tunnel_Packet packetRecv;

            if (recvfrom(listenPipe->listenerSocket, (void *)packetRecv.data, PACKET_SIZE - 2, 0, NULL, NULL) > 0 && clientStatus) {
                packetRecv.id = getAddrId(packetRecv.data, listenPipe);

                send(listenPipe->clientSocket, (void *)&packetRecv, PACKET_SIZE, 0);
            } else printf("pass or error packet \n");
        }
    }
}

void deployListenerThread(p_Listener_Pipe listenerPipe) {
    pthread_t pid;

    pthread_create(&pid, NULL, listenerThread, (void *)listenerPipe);
}
