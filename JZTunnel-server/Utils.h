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