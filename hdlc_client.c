#include "hdlc_client.h"

void run_client(int sock, struct sockaddr* server, socklen_t len, FILE* file) {
	len_send = build_sabm(buf_send);
	while (1) {
		sendto(sock, buf_send, len_send, 0, server, len);
		// timeout e blá
		len_recv = recvfrom(sock, buf_recv, MAX_BUFFER, 0, server, &len);
		// se for UA e tiver nos conforme, break
	}
}
