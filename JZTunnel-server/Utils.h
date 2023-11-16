#pragma once

// return 1 if error
int checkSocket(SOCKET sock_fd) {
    if (sock_fd < 0) {
        return 1;
    }
    return 0;
}

int compareIp(char *recvip, char *checkIp) {
    if (strlen(recvip) != strlen(checkIp)) {
        return FALSE;
    }

    for (int i = 0; i < strlen(recvip); i++) {
        if (recvip[i] != checkIp[i]) {
            return FALSE;
        }
    }

    return TRUE;
}

int checkStr(char * string) {
    char name[7] = "CLIENT\0";
    for (int i = 0; i < 7; i++) {
        if (name[i] != string[i]) return FALSE;
    }
    return TRUE;
}

int verifyHandshake(p_Handshake_Packet packet) {
    if (checkStr(packet->name) && packet->pass == PASS) {
        return TRUE;
    }
    return FALSE;
}

void cleanClientSocket(SOCKET clientSock, p_Listener_Pipe listenPipe) {
    clientStatus = FALSE;
    shutdown(clientSock, SHUT_RDWR);
    close(clientSock);

    listenPipe->clientSocket = SOCKET_ERROR;
    memset(listenPipe->addresses, 0, sizeof(address) * listenPipe->addrLen);
    listenPipe->addrLen = 0;
    listenPipe->addresses = (p_address)realloc(listenPipe->addresses, 0);
}

p_Listener_Pipe getListenerPipe(SOCKET listenSocket) {
    p_Listener_Pipe listenpipe = (p_Listener_Pipe)malloc(sizeof(Listener_Pipe));
    listenpipe->addresses = (p_address)malloc(0);
    listenpipe->addrLen = 0;
    listenpipe->clientSocket = SOCKET_ERROR;
    listenpipe->listenerSocket = listenSocket;

    return listenpipe;
}

short int getAddrId(struct sockaddr_in *recvAddr, p_Listener_Pipe listenpipe) {
    char *ip = inet_ntoa(recvAddr->sin_addr);
    unsigned short int port = htons(recvAddr->sin_port);

    printf("Packet recieved from:\nIP: %s\nPort: %u\n", ip, port);

    int i;
    for (i = 0; i < listenpipe->addrLen; i++) {
        if (port == listenpipe->addresses[i].port && compareIp(ip, listenpipe->addresses[i].ip)) {
            return i;
        }
    }

    listenpipe->addresses = (p_address)realloc(listenpipe->addresses, ++listenpipe->addrLen * sizeof(address));
    strcpy(listenpipe->addresses[i].ip, ip);
    listenpipe->addresses[i].port = port;

    return i;
}