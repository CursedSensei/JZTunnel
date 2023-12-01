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

p_Listener_Pipe getListenerPipe() {
    p_Listener_Pipe listenpipe = (p_Listener_Pipe)malloc(sizeof(Listener_Pipe));
    listenpipe->addresses = (p_address)malloc(0);
    listenpipe->addrLen = 0;
    listenpipe->clientSocket = SOCKET_ERROR;
    listenpipe->listenerSocket = SOCKET_ERROR;
    listenpipe->listenerBinder = SOCKET_ERROR;
    memset(&listenpipe->listenerAddr, 0, sizeof(struct in_addr));

    return listenpipe;
}

short int getAddrId(char *recv_buf, p_Listener_Pipe listenpipe) {
    p_IP_Header recv_header = (p_IP_Header)recv_buf;

    char *ip = inet_ntoa(recv_header->src_addr);

    p_UDP_Header porthdr = (p_UDP_Header)(recv_buf + 20);
    unsigned short int port = htons(porthdr->src_port);

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

int checkINET(char * buf, int index) {
    char check[] = "inet ";

    for (int i = 0; i < 5; i++) {
        if (buf[index + i] != check[i]) return FALSE;
    }

    return TRUE;
}

int checkLocalHost(char *ip) {
    if (strlen(ip) != 9) return FALSE;

    char check[] = "127.0.0.1";
    for (int i = 0; i < strlen(ip); i++) {
        if (ip[i] != check[i]) return FALSE;
    }

    return TRUE;
}

char * getNetIp() {
    static char outIp[16] = "";

    FILE *pipe;
    char ip[10][21];
    short int cur_ip = 0;
    char buf[250];

    pipe = popen("ip a", "r");

    while (fgets(buf, 250, pipe) != NULL) {
        int i = 0;
        while (buf[i] == ' ') i++;

        if (!checkINET(buf, i)) {
            continue;
        }

        i += 5;

        for (int d = 0; d < 21; d++) {
            if (buf[i + d] == '/') {
                ip[cur_ip][d] = '\0';
                break;
            }
            else ip[cur_ip][d] = buf[i + d];
        }
        cur_ip++;

    }

    pclose(pipe);

    for (int i = 0; i < cur_ip; i++) {
        if(!checkLocalHost(ip[i])) {
            strcpy(outIp, ip[i]);
            break;
        }
    }
    
    return outIp;
}

int checkPacket(char * packet, p_Listener_Pipe listenPipe) {
    p_IP_Header ipdata = (p_IP_Header)(packet + sizeof(struct ether_header));

    switch (ipdata->protocol) {
        case IPPROTO_TCP:
            {
                p_TCP_Header protodata = (p_TCP_Header)(packet + sizeof(struct ether_header) + sizeof(IP_Header));
                
                if (protodata->dest_port != TUNNEL_PORT) return FALSE;
            }
            break;

        case IPPROTO_UDP:
            {
                p_UDP_Header protodata = (p_UDP_Header)(packet + sizeof(struct ether_header) + sizeof(IP_Header));
                
                if (protodata->dest_port != TUNNEL_PORT) return FALSE;
            }
            break;
        default:
            return FALSE;
    }

    if (ipdata->dest_addr.s_addr != listenPipe->listenerAddr.s_addr) return FALSE;

    return TRUE;
}