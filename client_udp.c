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
	int c, sock, length, n;
	struct sockaddr_in server, from;
	struct hostent *hp;
	char buffer[256], *host, *filename;
	FILE* file;
	
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
	
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) error("socket");
	
	server.sin_family = AF_INET;
	hp = gethostbyname(host);
	if (hp == 0) error("Unknown host");
	
	file = fopen(filename, "r");
	if (file == 0) error("file");
	
	bcopy((char *)hp->h_addr, (char *)&server.sin_addr, hp->h_length);
	server.sin_port = htons(CONN_PORT);
	length=sizeof(struct sockaddr_in);
	printf("Please enter the message: ");
	bzero(buffer,256);
	fgets(buffer,255,stdin);
	n=sendto(sock,buffer,strlen(buffer),0,&server,length);
	if (n < 0) error("Sendto");
	n = recvfrom(sock,buffer,256,0,&from, &length);
	if (n < 0) error("recvfrom");
	write(1,"Got an ack: ",12);
	write(1,buffer,n);
}

void error(char *msg) {
	 perror(msg);
	 exit(1);
}
