#pragma once

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define SOCKET int
#define SOCKET_ERROR -1
#define PTHREAD_FUNCTION void*

enum bool {FALSE, TRUE};
static short int clientStatus = FALSE;

#define TUNNEL_PORT 30000
#define PASS 0xDE4A5FBA
#define PACKET_SIZE 0x5DC

#include "Socket_Structs.h"
#include "Utils.h"
#include "Socket.h"