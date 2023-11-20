#pragma once

#include <winsock2.h>
#include <stdio.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")

#define TUNNEL_IP "input domain name / IP"
#define TUNNEL_PORT "25565"
#define CLIENT_ERROR 0x50
#define PACKET_SIZE 0x5DC

#include "Socket_Structs.h"
#include "Utils.h"
#include "Sockets.h"