#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hdlc.h"
#include "hdlc_server.h"
#include "hdlc_client.h"

void error(char *);
int main(int argc, char **argv) {
	int c;
	struct sockaddr_in server;
	char *host = NULL, *filename = NULL;
	
	if (argc == 1) {
		printf("Usage: sim_hdlc -h ip [-f filename]\n");
		exit(1);
	}
	
	opterr = 0;
	while ((c = getopt(argc, argv, "h:f")) != -1) {
		switch(c) {
			case 'h':
				host = optarg; break;
			case 'f':
				filename = optarg; break;
			case '?':
			default:
				printf("Usage: sim_hdlc -h ip [-f filename]\n"); exit(1);
		}
	}
	
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) error("socket");
	
	server.sin_family = AF_INET;
	struct hostent* hp = gethostbyname(host);
	if (hp == 0) error("Unknown host");
	
	memcpy(&server.sin_addr, hp->h_addr, hp->h_length);
	server.sin_port = htons(CONN_PORT);
	socklen_t length = sizeof(struct sockaddr_in);
	
	if (filename == NULL) {
		run_server(sock, (struct sockaddr*)&server, length);
	} else {
		FILE* file = fopen(filename, "r");
		if (file == 0) error("file");
		run_client(sock, (struct sockaddr*)&server, length, file);
		fclose(file);
	}
	
	return 0;
}

void error(char *msg) {
	 perror(msg);
	 exit(1);
}
