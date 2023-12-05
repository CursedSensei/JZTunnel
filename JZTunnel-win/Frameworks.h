#pragma once

#include <winsock2.h>
#include <stdio.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")

#define TUNNEL_IP "input domain name / IP"
#define TUNNEL_PORT "443"
#define CLIENT_ERROR 0x5000
#define PACKET_SIZE 0x5DC

#define __DEBUG__ 1 // switches from debug view or user view

ULONG LOCAL_IP = inet_addr("127.0.0.1");

#include "Socket_Structs.h"
#include "Utils.h"
#include "Sockets.h"