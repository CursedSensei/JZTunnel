#pragma once

// return 1 if error
int checkSocket(SOCKET sock_fd) {
    if (sock_fd < 0) {
        return 1;
    }
    return 0;
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

short int getAddrId(unsigned char *recv_buf, p_Listener_Pipe listenpipe) {
    p_IP_Header recv_header = (p_IP_Header)(recv_buf + sizeof(struct ether_header));

    p_UDP_Header porthdr = (p_UDP_Header)(recv_buf + sizeof(IP_Header));

#if __DEBUG__
    printf("Packet recieved from IP: %s:%u\n", inet_ntoa(recv_header->src_addr), htons(porthdr->src_port));
#endif

    int i;
    for (i = 0; i < listenpipe->addrLen; i++) {
        if (porthdr->src_port == listenpipe->addresses[i].port && recv_header->src_addr.s_addr == listenpipe->addresses[i].ip.s_addr) {
            return i + 1;
        }
    }

    listenpipe->addresses = (p_address)realloc(listenpipe->addresses, ++listenpipe->addrLen * sizeof(address));
    listenpipe->addresses[i].ip.s_addr, recv_header->dest_addr.s_addr;
    listenpipe->addresses[i].port = porthdr->dest_port;

    struct ether_header* ether_hdr = (struct ether_header*)recv_buf;

    memcpy(&listenpipe->addresses[i].mac, &ether_hdr->ether_shost, 6);

    return i + 1;
}

int checkINET(char * buf, int index, const char *check) {
    for (int i = 0; i < strlen(check); i++) {
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

uint8_t hexConvert(char high_order, char lower_order) {
    uint8_t output = 0;

    if (high_order >= 'a' && high_order <= 'f') {
        output += (high_order - 87) << 4;
    } else {
        output += (high_order - '0') << 4;
    }

    if (lower_order >= 'a' && lower_order <= 'f') {
        output += (lower_order - 87);
    } else {
        output += (lower_order - '0');
    }

    return output;
}


void getNetIp(p_Listener_Pipe listenpipe) {
    FILE *pipe;
    char ip[10][21];
    char mac[10][18];
    short int cur_ip = 0;
    char buf[250];
    char macbuf[18];

    pipe = popen("ip a", "r");

    while (fgets(buf, 250, pipe) != NULL) {
        int i = 0;
        while (buf[i] == ' ') i++;

        if (!checkINET(buf, i, "inet ")) {
            if (checkINET(buf, i, "link/ether ")) {
                i += 11;

                for (int d = 0; d < 18; d++) {
                    macbuf[d] = buf[i + d];
                }
            }
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
        strcpy(mac[cur_ip], macbuf);
        cur_ip++;

    }

    pclose(pipe);

    for (int i = 0; i < cur_ip; i++) {
        if(!checkLocalHost(ip[i])) {
            listenpipe->listenerAddr.s_addr = inet_addr(ip[i]);

            for (int j = 0; j < 17; j += 3) {
                listenpipe->listenerMac[j / 3] = hexConvert(mac[i][j], mac[i][j + 1]);
            }

            break;
        }
    }
}

int checkPacket(unsigned char * packet, p_Listener_Pipe listenPipe) {
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

uint16_t checksum(uint16_t *buf, int len) {
    uint32_t sum = 0;
    while (len > 1) {
        sum += *buf++;
        len -= 2;
    }
    if (len) sum += *(uint8_t *)buf;

    while (sum >> 16) {
        sum = (sum & 0xffff) + (sum >> 16);
    }

    return ~sum;
}

uint16_t checksum_with_pseudo(uint16_t *pseudo_hrd, uint16_t *buf, int len) {
    uint32_t sum = 0;
    int hdr_len = sizeof(Pseudo_Header);
    while (hdr_len) {
        sum += *pseudo_hrd++;
        hdr_len -= 2;
    }
    while (len > 1) {
        sum += *buf++;
        len -= 2;
    }
    if (len) sum += *(uint8_t *)buf;

    while (sum >> 16) {
        sum = (sum & 0xffff) + (sum >> 16);
    }

    return ~sum;
}