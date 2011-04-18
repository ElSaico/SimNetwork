#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hdlc.h"

void error(char *);
int main(int argc, char **argv) {
	int c, len_send, len_recv;
	struct sockaddr_in server, from;
	uint8_t buf_send[256], buf_recv[256]; // aumentar buffers por conta do escaping?
	char *host = NULL, *filename = NULL;
	
	opterr = 0;
	while ((c = getopt(argc, argv, "h:f:")) != -1) {
		switch(c) {
			case 'h':
				host = optarg; break;
			case 'f':
				filename = optarg; break;
			case '?':
			default:
				printf("Usage: client -h ip -f filename\n"); exit(1);
		}
	}
	
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) error("socket");
	
	server.sin_family = AF_INET;
	struct hostent* hp = gethostbyname(host);
	if (hp == 0) error("Unknown host");
	
	FILE* file = fopen(filename, "r");
	if (file == 0) error("file");
	
	bcopy((char *)hp->h_addr, (char *)&server.sin_addr, hp->h_length);
	server.sin_port = htons(CONN_PORT);
	socklen_t length = sizeof(struct sockaddr_in);
	
	len_send = build_sabm(buf_send);
	while (1) {
		sendto(sock, buf_send, len_send, 0, (struct sockaddr*)&server, length);
		// timeout e blá
		len_recv = recvfrom(sock, buf_recv, 256, 0, (struct sockaddr*)&from, &length);
		// se for UA e tiver nos conforme, break
	}
	
	return 0;
}

void error(char *msg) {
	 perror(msg);
	 exit(1);
}
