#pragma once

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define SOCKET int
#define SOCKET_ERROR -1
#define PTHREAD_FUNCTION void*

enum bool {FALSE, TRUE};
short int clientStatus = FALSE;

uint16_t TUNNEL_PORT = htons(30000);
const int PACKET_SIZE = 0x5DC;

#define PASS 0xDE4A5FBA
#define CLIENT_PORT 443 // soon to change

#include "Socket_Structs.h"
#include "Utils.h"
#include "Socket.h"