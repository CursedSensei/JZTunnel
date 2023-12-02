#include "Frameworks.h"

int main() {
	p_Listener_Pipe listenPipe = getListenerPipe();
	getListenerSocket(listenPipe);
	deployListenerThread(listenPipe);

	while (TRUE) {
		printf("Waiting for client\n");

		SOCKET clientSocket = getClientSocket(listenPipe->listenerSocket);
		if (checkSocket(clientSocket)) continue;
		else clientStatus = TRUE;

		getNetIp(listenPipe);

		printf("Connected to client\n");

		Tunnel_Packet clientPacket;
		struct ether_header* ether_hdr = (struct ether_header*)clientPacket.data;
		p_IP_Header ip_hdr = (p_IP_Header)(clientPacket.data + sizeof(struct ether_header));

		p_TCP_Header tcp_hdr = (p_TCP_Header)(clientPacket.data + sizeof(struct ether_header) + sizeof(IP_Header));
		p_UDP_Header udp_hdr = (p_UDP_Header)(clientPacket.data + sizeof(struct ether_header) + sizeof(IP_Header));

		Pseudo_Header pseudo_ip;
		pseudo_ip.reserved = 0;

		for (int recvStatus = 0; recvStatus >= 0; recvStatus = recv(clientSocket, (void *)&clientPacket, PACKET_SIZE, 0)) {

			if (recvStatus > 0) {
				memcpy(ether_hdr->ether_shost, listenPipe->listenerMac, 6);
				memcpy(ether_hdr->ether_dhost, listenPipe->addresses[clientPacket.id].mac, 6);

				ip_hdr->src_addr.s_addr = listenPipe->listenerAddr.s_addr;
				ip_hdr->dest_addr.s_addr = listenPipe->addresses[clientPacket.id].ip.s_addr;
				ip_hdr->ttl = 64;
				ip_hdr->checksum = 0;

				ip_hdr->checksum = checksum((uint16_t *)ip_hdr, sizeof(IP_Header));

				switch (ip_hdr->protocol) {
					case IPPROTO_TCP:
						{
							pseudo_ip.dest_addr = ip_hdr->dest_addr.s_addr;
							pseudo_ip.src_addr = ip_hdr->src_addr.s_addr;
							pseudo_ip.protocol = IPPROTO_TCP;
							pseudo_ip.length = htons(recvStatus - 2 - sizeof(struct ether_header) - sizeof(IP_Header));

							tcp_hdr->src_port = TUNNEL_PORT;
							tcp_hdr->dest_port = listenPipe->addresses[clientPacket.id].port;
							tcp_hdr->checksum = 0;

							tcp_hdr->checksum = checksum_with_pseudo((uint16_t *)&pseudo_ip,
								(uint16_t *)&tcp_hdr,
								recvStatus - 2 - sizeof(struct ether_header) - sizeof(IP_Header));
						}
						break;
					case IPPROTO_UDP:
						{
							pseudo_ip.dest_addr = ip_hdr->dest_addr.s_addr;
							pseudo_ip.src_addr = ip_hdr->src_addr.s_addr;
							pseudo_ip.protocol = IPPROTO_UDP;
							pseudo_ip.length = htons(recvStatus - 2 - sizeof(struct ether_header) - sizeof(IP_Header));

							udp_hdr->src_port = TUNNEL_PORT;
							udp_hdr->dest_port = listenPipe->addresses[clientPacket.id].port;
							udp_hdr->checksum = 0;

							tcp_hdr->checksum = checksum_with_pseudo((uint16_t *)&pseudo_ip,
								(uint16_t *)&udp_hdr,
								recvStatus - 2 - sizeof(struct ether_header) - sizeof(IP_Header));
						}
						break;
					default:
						printf("Unsupported Protocol number: %u\n", ip_hdr->protocol);
				}

				send(listenPipe->listenerSocket, clientPacket.data, recvStatus - 2, 0);

#if __DEBUG__
    			printf("Packet sent to IP: %s\n", inet_ntoa(ip_hdr->dest_addr));
#endif

				memset(&clientPacket, 0, recvStatus);
			}
		}

		cleanClientSocket(clientSocket, listenPipe);
	}

	close(listenPipe->listenerSocket);
	close(listenPipe->listenerBinder);

	return 0;
}