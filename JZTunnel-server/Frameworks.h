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
#define PACKET_SIZED 0x5DC

enum bool {FALSE, TRUE};
short int clientStatus = FALSE;

const uint16_t TUNNEL_PORT = 0x3075; // 30000
const int PACKET_SIZE = PACKET_SIZED;

#define PASS 0xDE4A5FBA
#define CLIENT_PORT 443 // soon to change

#define __DEBUG__ 1 // Enable or Disable packet debug messages

#include "Socket_Structs.h"
#include "Utils.h"
#include "Socket.h"