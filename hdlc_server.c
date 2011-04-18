#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hdlc.h"

void error(char *msg) {
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[]) {
	if ((argc != 3) || (strcmp(argv[1], "-h") != 0)) {
		printf("Usage: hdlc_server -h ip\n");
		exit(1);
	}
	
	int len_send, len_recv;
	struct sockaddr_in server;
	uint8_t buf_send[256], buf_recv[256];
	
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) error("Opening socket");
	
	struct hostent* hp = gethostbyname(argv[2]);
	if (hp == 0) error("Unknown host");
	
	memcpy(&server.sin_addr, hp->h_addr, hp->h_length);	
	server.sin_family = AF_INET;
	server.sin_port = htons(CONN_PORT);
	socklen_t length = sizeof(server);
	
	if (bind(sock, (struct sockaddr *)&server, length) < 0) 
		error("binding");
	
	while (1) {
		//len_recv = recvfrom(sock,buf,1024,0,(struct sockaddr *)&server,&length);
		//len_send = sendto(sock,"Got your message\n",17,0,(struct sockaddr *)&server,length);
	}
	
	return 0;
}
