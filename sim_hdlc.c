#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "hdlc.h"
#include "hdlc_server.h"
#include "hdlc_client.h"

void error(char *);
int main(int argc, char **argv) {
	int c;
	HDLCSocket data;
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
	
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&received, NULL);
	
	data.sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (data.sock < 0) error("socket");
	
	data.sock_addr.sin_family = AF_INET;
	struct hostent* hp = gethostbyname(host);
	if (hp == 0) error("Unknown host");
	
	memcpy(&data.sock_addr.sin_addr, hp->h_addr, hp->h_length);
	data.sock_addr.sin_port = htons(CONN_PORT);
	data.sock_len = sizeof(struct sockaddr_in);
	
	srand(time(NULL));
	if (filename == NULL) {
		run_server(&data);
	} else {
		data.file = fopen(filename, "r");
		if (data.file == 0) error("file");
		run_client(&data);
		fclose(data.file);
	}
	
	pthread_cond_destroy(&received);
	pthread_mutex_destroy(&mutex);
	return 0;
}

void error(char *msg) {
	 perror(msg);
	 exit(1);
}
