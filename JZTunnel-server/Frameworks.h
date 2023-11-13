#pragma once

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SOCKET int
#define SOCKET_ERROR -1

enum bool {FALSE, TRUE};

#define TUNNEL_PORT 30000
#define PASS 0xDE4A5F

#include "Socket_Structs.h"
#include "Utils.h"
#include "Socket.h"